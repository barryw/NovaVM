; NOVAZ6.BIN — RAM-resident V6 segment (M1: minimal 8-window model).
;
; Linked at $2000 by src/novaz6.cfg, packed onto every NovaZ image as
; NOVAZ6.BIN, and loaded by the runtime ROM at boot when the story is
; version 6 (see nz6_load_segment in src/runtime.s).
;
; ABI contract with the ROM:
;   - The ROM has already fetched the opcode and decoded its operands into
;     zvm_operand_lo/hi and zvm_operand_count before dispatching here.
;   - Entry: the ROM handler JMPs (or JSRs) to NZ6_ENTRY ($2002) with
;     A = dispatch id (NZ6_OP_* / NZ6_EXT_BASE + ext opnum, see zvm6.inc).
;     Segment code RTSes back to the interpreter step loop exactly like a
;     ROM handler would.
;   - Segment code may call exported ROM routines (zvm_fetch, zvm_set_var,
;     branch helpers, ...) via the generated ABI include,
;     build/runtime_abi.inc, which pins each symbol to its ROM address.
;
; Layout is load-bearing: magic word at $2000, entry at $2002 — the ROM
; verifies the magic after loading and aborts the boot if it is wrong.
;
; ----------------------------------------------------------------------------
; Window model (M2, Z-Machine Standard 1.1 section 8.8):
;   - 8 windows, 16 word-sized properties each, stored little-endian in
;     nz6_win_props (offset = window*32 + prop*2).
;   - Coordinates/cursors are 1-based V6 UNITS, and 1 unit = 1 gfx pixel:
;     the header (zstory.s) advertises a 320x200-unit screen with a 4x4 font.
;     Text output converts units to 80x50 cells at the VTEXT boundary; picture
;     blits stay in native gfx pixels.
;   - The CURRENT window owns the live vtext region: every geometry/cursor
;     op on it recomputes VTEXT_LEFT/TOP/WIDTH/HEIGHT/CURX/CURY/FLAGS from
;     the prop table (nz6_apply_current_window). The ROM's classic
;     lower/upper split machinery (zvm_split_lines, zvm_lower_*/zvm_upper_*)
;     is v1-5-only: zvm_select_active_window's V6 branch routes here with
;     dispatch id NZ6_OP_SELECT instead. Cursor props 4/5 of the current
;     window are freshened from the live vtext cursor whenever the table is
;     consulted (window switch, get_cursor, get_wind_prop 4/5) or its
;     geometry changes; non-current windows live purely in the table.
;   - The NovaZ word-buffer/wrap/[MORE] layer in the ROM (nz_screen_*) reads
;     the LIVE vtext state, so it follows whatever rectangle is applied;
;     zvm_window_current degenerates to a buffer gate (0 = buffered window 0
;     output, 1 = raw) and carries no geometry.
;   - Out of M2 scope (stored but not acted on): newline interrupts (props
;     8/9 round-trip but never fire — Zork Zero arms countdown -1),
;     transcript bit, line counts, colour data (prop 11), font props,
;     window_style rendering beyond wrap/scroll attributes.

.setcpu "w65c02"

.include "zvm6.inc"
.include "nova.inc"             ; VGC register equates (VGC_PALETTE)
.include "xram.inc"             ; XRAM_ADDR*/XRAM_DATA pseudo-registers
.include "vgc_palette.inc"      ; vgc_set_palette_* helpers
.include "pager.inc"            ; PAGER_* args for the FIO XPAGE region loader
.include "zstory.inc"           ; XRAM map (pics index/bounce regions)
.include "runtime_abi.inc"      ; ROM ABI addresses; must follow zvm6.inc

; Screen geometry. Text cells are 4x4 V6 units/gfx pixels.
NZ6_SCREEN_ROWS = 50
NZ6_SCREEN_COLS = 80
NZ6_SCREEN_UNIT_ROWS = 200
NZ6_SCREEN_UNIT_COLS = 320
NZ6_CELL_UNIT_SHIFT = 2

; V6 starts in the VGC's EGA palette mode for v1 PICS.PAK compatibility.
; v2 packs upload a custom RGB palette at boot, preserving the same 4-bit
; colour-index contract. The default text colour remains index white-on-black
; — NOT the ROM's Nova-palette ZVM_COLOR_NORMAL ($0C = grey there).
NZ6_COLOR_DEFAULT = $0F
NZ6_PIC_FLAG_TRANSPARENT = $01
NZ6_PIC_FLAG_FLOW_ICON = $04
NZ6_PIC_FLAG_FLOW_CELLTOP = $08

; DMA/blitter memory-space ids (VgcConstants.DmaSpace*).
NZ6_SPACE_GFX  = $03
NZ6_SPACE_XRAM = $05
; The VGC gfx plane is addressed as one 4-bit pixel per address. PICS.PAK
; bitmaps stay row-packed in storage and are unpacked by the blitter.
NZ6_GFX_ROW_PIXELS = 320
NZ6_GFX_ROWS       = 200
NZ6_GFX_LIST_MAX   = 192
NZ6_GFX_OP_DRAW    = $01
NZ6_GFX_OP_ERASE   = $02
NZ6_WORD_SAVE_MAX  = 80

.segment "BSS"

; 8 windows x 16 properties x 2 bytes, little-endian words.
nz6_win_props:   .res 8*16*2
nz6_win_current: .res 1         ; current V6 window, 0-7
nz6_tmp_win:     .res 1         ; window targeted by the op in flight
nz6_tmp_off:     .res 1         ; scratch for table offset math
nz6_tmp_lines:   .res 1         ; split_window/scroll: clamped cell rows
nz6_tmp_mod:     .res 1         ; scratch: pixel offset inside a 4px text cell
nz6_unit_lo:     .res 1         ; 16-bit unit/value scratch
nz6_unit_hi:     .res 1
nz6_rect_left:   .res 1         ; erase_window: cell rect
nz6_rect_top:    .res 1
nz6_rect_w:      .res 1
nz6_rect_h:      .res 1
nz6_marg_l:      .res 1         ; apply: left/right margin cells in flight
nz6_marg_r:      .res 1
nz6_stk_lo:      .res 1         ; push_stack/pull: free-slot count in flight
nz6_stk_hi:      .res 1
nz6_colour:      .res 1         ; live set_colour state: bg high nibble, fg low
nz6_clr_tmp:     .res 1         ; scratch for style/nibble math
nz6_pics_avail:  .res 1         ; 1 = PICS.PAK index resident in XRAM
nz6_pics_version: .res 1        ; PICS.PAK format version (1 or 2)
nz6_pics_base_l:  .res 1        ; allocated XRAM arena: index +0, bounce +$2000
nz6_pics_base_m:  .res 1
nz6_pics_base_h:  .res 1
nz6_pics_index_l: .res 1        ; XRAM low byte of the first index entry
nz6_pics_count_lo: .res 1       ; picture count from the pak header (LE)
nz6_pics_count_hi: .res 1
nz6_pics_release_lo: .res 1     ; pak release number (LE)
nz6_pics_release_hi: .res 1
nz6_pics_cur_l:  .res 1         ; 24-bit XRAM cursor over the index
nz6_pics_cur_m:  .res 1
nz6_pics_cur_h:  .res 1
nz6_pics_rem_lo: .res 1         ; entries left to scan
nz6_pics_rem_hi: .res 1
nz6_pic_w_lo:    .res 1         ; matched entry: width px (LE)
nz6_pic_w_hi:    .res 1
nz6_pic_h_lo:    .res 1         ; height px
nz6_pic_h_hi:    .res 1
nz6_pic_flags:   .res 1         ; bit 0 transparency, bits 4-7 transparent index
nz6_pic_off_l:   .res 1         ; bitmap offset in the pak (24 bits used)
nz6_pic_off_m:   .res 1
nz6_pic_off_h:   .res 1
nz6_pic_len_lo:  .res 1         ; bitmap length (<= 32000)
nz6_pic_len_hi:  .res 1
nz6_pic_tmp:     .res 1         ; entry-scan scratch (z_number low byte)
nz6_blt_cellx:   .res 1         ; draw/erase: abs cell coords
nz6_blt_celly:   .res 1
nz6_blt_px_lo:   .res 1         ; draw/erase: abs gfx-pixel coords
nz6_blt_px_hi:   .res 1
nz6_blt_py:      .res 1
nz6_blt_dst_lo:  .res 1         ; gfx destination byte address
nz6_blt_dst_hi:  .res 1
nz6_blt_wbytes:  .res 1         ; full bitmap row bytes (src stride)
nz6_blt_wclip:   .res 1         ; erase: clipped rect width in PIXELS (16-bit)
nz6_blt_wclip_hi: .res 1
nz6_blt_hclip:   .res 1         ; erase: clipped rect height in rows
nz6_gfx_base_color: .res 1      ; full-plane fill colour before retained ops
nz6_gfx_list_count: .res 1      ; retained gfx ops since last full-plane clear
nz6_gfx_replay_active: .res 1   ; nonzero while replaying retained ops
nz6_gfx_replay_index: .res 1
nz6_gfx_replay_fill: .res 1
nz6_gfx_list_op: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_pic_lo: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_pic_hi: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_x_lo: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_x_hi: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_y: .res NZ6_GFX_LIST_MAX
nz6_gfx_list_fill: .res NZ6_GFX_LIST_MAX

.assert nz6_win_props = NZ6_SAVE_DISPLAY_STATE_BASE, error, "NZ6_SAVE_DISPLAY_STATE_BASE must point at V6 window state"
.assert nz6_colour + 1 - nz6_win_props = NZ6_SAVE_DISPLAY_STATE_SIZE, error, "NZ6_SAVE_DISPLAY_STATE_SIZE must cover V6 display state"
.assert nz6_gfx_base_color = NZ6_SAVE_GFX_STATE_BASE, error, "NZ6_SAVE_GFX_STATE_BASE must point at retained graphics state"
.assert nz6_gfx_list_fill + NZ6_GFX_LIST_MAX - nz6_gfx_base_color = NZ6_SAVE_GFX_STATE_SIZE, error, "NZ6_SAVE_GFX_STATE_SIZE must cover retained graphics state"

nz6_saved_word_len: .res 1
nz6_saved_word_buf: .res NZ6_WORD_SAVE_MAX

.segment "CODE"

        .byte NZ6_MAGIC0, NZ6_MAGIC1   ; $2000: magic word

; A = dispatch id (see zvm6.inc):
;   $00-$0C           VAR/ROM screen ops -> nz6_var_table
;   $20 + ext opnum   EXT ops -> nz6_ext_table (opnums 0-29)
; Dispatch via 65C02 JMP (abs,X): each handler RTSes straight back to the
; interpreter step loop, exactly like a ROM table handler would.
nz6_entry:                              ; $2002: dispatch entry, A = id
        JMP nz6_dispatch

.assert nz6_entry = NZ6_ENTRY, error, "nz6_entry must sit at NZ6_ENTRY ($2002)"

nz6_dispatch:
        CMP #NZ6_EXT_BASE
        BCS @ext
        CMP #NZ6_OP_RESTORE_DISPLAY + 1
        BCS nz6_bug             ; reserved VAR ids $0C-$1F are never routed
        ASL
        TAX
        JMP (nz6_var_table,X)
@ext:
        SBC #NZ6_EXT_BASE       ; carry known set from the BCS above
        CMP #NZ6_EXT_COUNT
        BCS nz6_bug             ; ext opnum >= 30 is never routed
        ASL
        TAX
        JMP (nz6_ext_table,X)

; Ids the ROM never routes here (VAR ids > $07; ext opnums 0-4 and 9-15 stay
; in the ROM's own table). Reaching this is an ROM<->segment ABI contract
; violation — fail loudly through the ROM's unsupported-opcode path rather
; than silently no-opping a routing bug.
nz6_bug:
        JMP zvm_unsupported

; Plain no-op stub: all operands were already decoded by the ROM and the op
; has no store byte and no branch, so ignoring it is stream-safe.
nz6_stub:
        RTS

; Store-op stub (buffer_screen EXT:29): MUST consume the store byte or the
; instruction stream derails. Mirrors the ROM's zvm_false_store contract:
; value in zvm_value_lo/hi, zvm_fetch returns the store variable number in A,
; zvm_set_var stores it.
nz6_store_zero:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JSR zvm_fetch
        JMP zvm_set_var

; Branch-op stub (picture_data EXT:6, make_menu EXT:27): zvm_branch_false
; itself runs zvm_branch_decode (consuming the branch byte(s)) and then
; applies condition "false" — do NOT pre-decode here or the branch bytes
; would be consumed twice.
nz6_branch_false:
        JMP zvm_branch_false

; Optional NDK mixed-plane scroll helper. Including this source here lets the
; RAM-resident V6 segment use the shared implementation while resolving VTEXT
; and blitter calls through runtime_abi.inc instead of linking private copies
; of the resident runtime objects.
.include "vtext_mixed.s"

; --- Window table helpers ----------------------------------------------------

; Y := nz6_tmp_win*32 + X*2 (window 0-7 in nz6_tmp_win, prop 0-15 in X).
; Preserves X. Clobbers A.
nz6_off_for_prop:
        LDA nz6_tmp_win
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        STA nz6_tmp_off
        TXA
        ASL A
        ORA nz6_tmp_off         ; prop*2 <= 30 < 32: no carry into the window bits
        TAY
        RTS

; Prop X of window nz6_tmp_win -> nz6_unit_lo/hi. Preserves X.
nz6_read_prop_unit:
        JSR nz6_off_for_prop
        LDA nz6_win_props,Y
        STA nz6_unit_lo
        LDA nz6_win_props+1,Y
        STA nz6_unit_hi
        RTS

; nz6_unit_lo/hi -> prop X of window nz6_tmp_win. Preserves X.
nz6_write_prop_unit:
        JSR nz6_off_for_prop
        LDA nz6_unit_lo
        STA nz6_win_props,Y
        LDA nz6_unit_hi
        STA nz6_win_props+1,Y
        RTS

; nz6_unit := 1 (or 320): origin and full-width constants; units are pixels.
nz6_unit_one:
        LDA #1
        STA nz6_unit_lo
        STZ nz6_unit_hi
        RTS

nz6_unit_80:
        LDA #<NZ6_SCREEN_UNIT_COLS
        STA nz6_unit_lo
        LDA #>NZ6_SCREEN_UNIT_COLS
        STA nz6_unit_hi
        RTS

; Window number from operand index X -> A (0-7), C=1 valid / C=0 invalid.
; V6 also uses -3 ($FFFD) to mean "the current window" for window ops.
; Preserves X.
nz6_window_from_operand:
        LDA zvm_operand_hi,X
        CMP #$FF
        BNE @non_negative
        LDA zvm_operand_lo,X
        CMP #$FD
        BNE @bad
        LDA nz6_win_current
        SEC
        RTS
@non_negative:
        LDA zvm_operand_hi,X
        BNE @bad
        LDA zvm_operand_lo,X
        CMP #8
        BCS @bad
        SEC
        RTS
@bad:
        CLC
        RTS

; --- unit <-> cell conversions -------------------------------------------------

; cell = (unit - 1) / 4 for the 16-bit unit in nz6_unit_lo/hi -> A.
; unit 0 (never legal, units are 1-based) maps to cell 0; results past 255
; clamp to $FF. Clobbers nz6_unit.
nz6_unit_to_cell:
        LDA nz6_unit_lo
        BNE :+
        LDA nz6_unit_hi
        BEQ @zero
:
        SEC
        LDA nz6_unit_lo
        SBC #1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC #0
        STA nz6_unit_hi
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        RTS
@zero:
        LDA #0
        RTS

; cell = ceil(unit / 4) for a 0-based pixel edge in nz6_unit_lo/hi -> A.
; Used when snapping a pixel window's leading edge inward to full text cells.
nz6_unit_edge_start_cell:
        CLC
        LDA nz6_unit_lo
        ADC #3
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC #0
        STA nz6_unit_hi
        BRA nz6_unit_edge_end_cell

; cell = floor(unit / 4) for a 0-based exclusive pixel edge in
; nz6_unit_lo/hi -> A.
nz6_unit_edge_end_cell:
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        RTS

; Prop X (a 1-based origin) -> nz6_unit = 0-based pixel origin.
nz6_prop_origin_zero:
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        BNE :+
        LDA nz6_unit_hi
        BEQ @done
:
        SEC
        LDA nz6_unit_lo
        SBC #1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC #0
        STA nz6_unit_hi
@done:
        RTS

; cells = ceil(unit / 4) for the 16-bit size in nz6_unit_lo/hi -> A.
; Size 0 maps to 0. Results past 255 clamp to $FF. Clobbers nz6_unit.
nz6_unit_size_to_cells:
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BEQ @zero
        CLC
        LDA nz6_unit_lo
        ADC #3
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC #0
        STA nz6_unit_hi
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        RTS
@zero:
        LDA #0
        RTS

; unit = cell*4 + 1 for the cell in A -> nz6_unit_lo/hi.
nz6_cell_to_unit:
        STZ nz6_unit_hi
        ASL A
        ROL nz6_unit_hi
        ASL A
        ROL nz6_unit_hi
        INC A
        STA nz6_unit_lo
        BNE :+
        INC nz6_unit_hi                 ; cell $FF -> unit $100
:
        RTS

; Prop X of window nz6_tmp_win -> A, saturated to $FF when the stored word
; exceeds one byte (screen geometry fits in a byte; the table may legally
; hold any 16-bit value). Preserves X.
nz6_prop_byte:
        JSR nz6_read_prop_unit
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        RTS

; Prop X (a 1-based unit) of window nz6_tmp_win -> A = 0-based cell,
; saturated to $FF. Preserves X.
nz6_prop_cell:
        JSR nz6_read_prop_unit
        JMP nz6_unit_to_cell

; Prop X (a 1-based vertical origin) -> A = first owned text row. Normal
; windows use the floor cell. Near the top edge, a partial-cell origin would
; otherwise let a tiny status/header window and the following main window both
; render into row 1, so snap only that case down to the next row.
nz6_prop_top_cell:
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        BNE :+
        LDA nz6_unit_hi
        BEQ @zero
:
        SEC
        LDA nz6_unit_lo
        SBC #1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC #0
        STA nz6_unit_hi
        LDA nz6_unit_lo
        AND #$03
        STA nz6_tmp_mod
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        CMP #2
        BCS @done
        LDX nz6_tmp_mod
        BEQ @done
        INC A
@done:
        RTS
@zero:
        LDA #0
        RTS

; Prop X (a unit size/count) of window nz6_tmp_win -> A = cell count,
; saturated to $FF. Preserves X.
nz6_prop_size_cells:
        JSR nz6_read_prop_unit
        JMP nz6_unit_size_to_cells

; --- the current window IS the live vtext region -------------------------------

; Rebuild the live vtext region from the CURRENT window's props and
; reposition the visible cursor. This replaces the ROM's lower/upper split
; logic for V6: zvm_select_active_window routes here (NZ6_OP_SELECT), and
; every segment op that changes the current window's geometry or cursor
; tail-calls it. Mapping:
;   VTEXT_TOP    = snapped text row for prop0          clamped onto the screen
;   VTEXT_LEFT   = (prop1 - 1) / 4 + left margin      clamped onto the screen
;   VTEXT_HEIGHT = ceil(prop2 / 4)          clamped to the rows below TOP
;   VTEXT_WIDTH  = ceil(prop3 / 4) - margins clamped to the cols right of LEFT
;   VTEXT_FLAGS  = attr (prop14) & 3 — Z attribute bit 0 (wrap) and bit 1
;                  (scroll) line up exactly with VTEXT_FLAG_WRAP/SCROLL.
;                  Window 0 defaults to both, windows 1-7 to neither.
;   VTEXT_CURX/Y = props 5/4 - 1, clamped inside WIDTH/HEIGHT.
; Cursor props 4/5 are 1-based and relative to the margin-shrunk writable
; area. A zero-sized window leaves an invalid region: vtext drops output
; until the game sizes it (games size windows before drawing into them).
nz6_apply_current_window:
        JSR nz6_apply_colour_style
        LDA nz6_win_current
        STA nz6_tmp_win
        JSR nz6_build_region_tmp_win
        ; cursor (skip for a degenerate region — vtext rejects it anyway)
        LDA VTEXT_HEIGHT
        BEQ @done
        LDA VTEXT_WIDTH
        BEQ @done
        LDX #4
        JSR nz6_prop_cell
        CMP VTEXT_HEIGHT
        BCC :+
        LDA VTEXT_HEIGHT
        DEC A
:
        STA VTEXT_CURY
        LDX #5
        JSR nz6_prop_cell
        CMP VTEXT_WIDTH
        BCC :+
        LDA VTEXT_WIDTH
        DEC A
:
        STA VTEXT_CURX
        JMP vtext_set_cursor
@done:
        RTS

; Geometry-only half of the rebuild: window nz6_tmp_win's margin-shrunk
; writable rect + flags -> the live VTEXT_LEFT/TOP/WIDTH/HEIGHT/FLAGS. Does
; NOT touch the cursor — callers borrowing a non-current window's rect
; (scroll_window) use it directly and restore via nz6_apply_current_window.
;
; Frotz renders V6 text into a pixel window. Nova's text plane is coarser
; (4x4 pixels per cell), so a non-cell-aligned left edge must not let opaque
; text-background cells overwrite adjacent chrome. Include the origin's
; intra-cell offset in the left margin calculation even when the story's
; explicit left margin is zero; shrink width by that same cell count so the
; right edge stays stable.
nz6_build_region_tmp_win:
        ; TOP
        LDX #0
        JSR nz6_prop_top_cell
        CMP #NZ6_SCREEN_ROWS
        BCC :+
        LDA #NZ6_SCREEN_ROWS-1
:
        STA VTEXT_TOP
        ; margins. Left margin starts at window-origin+margin pixels; include
        ; the origin's intra-cell offset before snapping to the 4px text grid
        ; so a zero-margin unaligned window still starts inside its pixel edge.
        LDX #1
        JSR nz6_read_prop_unit
        SEC
        LDA nz6_unit_lo
        SBC #1
        AND #$03
        STA nz6_tmp_mod
        LDX #6
        JSR nz6_read_prop_unit
        CLC
        LDA nz6_unit_lo
        ADC nz6_tmp_mod
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC #0
        STA nz6_unit_hi
        JSR nz6_unit_size_to_cells
        STA nz6_marg_l
        LDX #7
        JSR nz6_prop_size_cells
        STA nz6_marg_r
        ; LEFT = origin cell + snapped left margin
        LDX #1
        JSR nz6_prop_cell
        CLC
        ADC nz6_marg_l
        BCS @left_clamp
        CMP #NZ6_SCREEN_COLS
        BCC :+
@left_clamp:
        LDA #NZ6_SCREEN_COLS-1
:
        STA VTEXT_LEFT
        ; HEIGHT = prop2, clamped to the rows below TOP
        LDX #2
        JSR nz6_prop_size_cells
        STA VTEXT_HEIGHT
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC VTEXT_TOP
        CMP VTEXT_HEIGHT
        BCS :+
        STA VTEXT_HEIGHT
:
        ; WIDTH = prop3 - snapped margins (floor 0), clamped to the cols
        ; right of LEFT.
        LDX #3
        JSR nz6_prop_size_cells
        SEC
        SBC nz6_marg_l
        BCC @width_zero
        SBC nz6_marg_r          ; carry still set when the first SBC held
        BCS @width_have
@width_zero:                            ; reached by the BCC above AND by
        LDA #0                          ; fall-through when marg_r borrows
@width_have:
        STA VTEXT_WIDTH
        LDA #NZ6_SCREEN_COLS
        SEC
        SBC VTEXT_LEFT
        CMP VTEXT_WIDTH
        BCS :+
        STA VTEXT_WIDTH
:
        ; FLAGS from the window attributes
        LDX #14
        JSR nz6_prop_byte
        AND #$03
        STA VTEXT_FLAGS
        RTS

; Geometry-only rect for destructive full-window erases. Own only complete text
; cells inside the pixel window; adjacent non-cell-aligned V6 windows can touch
; the same 4px text cell, and erase_window must not wipe a neighbour's column.
nz6_build_inner_region_tmp_win:
        ; TOP
        LDX #0
        JSR nz6_prop_origin_zero
        JSR nz6_unit_edge_start_cell
        CMP #NZ6_SCREEN_ROWS
        BCC :+
        LDA #NZ6_SCREEN_ROWS-1
:
        STA VTEXT_TOP
        ; LEFT
        LDX #1
        JSR nz6_prop_origin_zero
        JSR nz6_unit_edge_start_cell
        CMP #NZ6_SCREEN_COLS
        BCC :+
        LDA #NZ6_SCREEN_COLS-1
:
        STA VTEXT_LEFT
        ; HEIGHT = floor((origin_y - 1 + prop2) / 4) - TOP, clamped
        ; to the rows below TOP. Full-window operations own only complete
        ; text cells inside the pixel window; otherwise adjacent windows that
        ; meet on a non-4px edge both touch the same text cell.
        LDX #0
        JSR nz6_prop_origin_zero
        LDA nz6_unit_lo
        STA nz6_stk_lo
        LDA nz6_unit_hi
        STA nz6_stk_hi
        LDX #2
        JSR nz6_read_prop_unit
        CLC
        LDA nz6_unit_lo
        ADC nz6_stk_lo
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC nz6_stk_hi
        STA nz6_unit_hi
        JSR nz6_unit_edge_end_cell
        SEC
        SBC VTEXT_TOP
        BCS :+
        LDA #0
:
        STA VTEXT_HEIGHT
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC VTEXT_TOP
        CMP VTEXT_HEIGHT
        BCS :+
        STA VTEXT_HEIGHT
:
        ; WIDTH = floor((origin_x - 1 + prop3) / 4) - LEFT, clamped
        ; to the cols right of LEFT.
        LDX #1
        JSR nz6_prop_origin_zero
        LDA nz6_unit_lo
        STA nz6_stk_lo
        LDA nz6_unit_hi
        STA nz6_stk_hi
        LDX #3
        JSR nz6_read_prop_unit
        CLC
        LDA nz6_unit_lo
        ADC nz6_stk_lo
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC nz6_stk_hi
        STA nz6_unit_hi
        JSR nz6_unit_edge_end_cell
        SEC
        SBC VTEXT_LEFT
        BCS :+
        LDA #0
:
        STA VTEXT_WIDTH
        LDA #NZ6_SCREEN_COLS
        SEC
        SBC VTEXT_LEFT
        CMP VTEXT_WIDTH
        BCS :+
        STA VTEXT_WIDTH
:
        ; FLAGS from the window attributes
        LDX #14
        JSR nz6_prop_byte
        AND #$03
        STA VTEXT_FLAGS
        RTS

nz6_build_full_region_tmp_win:
        ; TOP = first complete text row inside the pixel window. Composite
        ; scrolling moves existing text, so it must not borrow the chrome row
        ; above a non-cell-aligned V6 window.
        LDX #0
        JSR nz6_prop_origin_zero
        JSR nz6_unit_edge_start_cell
        CMP #NZ6_SCREEN_ROWS
        BCC :+
        LDA #NZ6_SCREEN_ROWS-1
:
        STA VTEXT_TOP
        ; LEFT = origin cell + one-cell inward snap for unaligned windows.
        ; Keep scroll/clear ownership aligned with normal text output.
        LDX #1
        JSR nz6_read_prop_unit
        SEC
        LDA nz6_unit_lo
        SBC #1
        AND #$03
        BEQ @full_left_aligned
        LDA #1
        BRA @full_left_have_extra
@full_left_aligned:
        LDA #0
@full_left_have_extra:
        STA nz6_marg_l
        LDX #1
        JSR nz6_prop_cell
        CLC
        ADC nz6_marg_l
        BCS @full_left_clamp
        CMP #NZ6_SCREEN_COLS
        BCC :+
@full_left_clamp:
        LDA #NZ6_SCREEN_COLS-1
:
        STA VTEXT_LEFT
        ; HEIGHT = prop2, clamped to the rows below TOP
        LDX #2
        JSR nz6_prop_size_cells
        STA VTEXT_HEIGHT
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC VTEXT_TOP
        CMP VTEXT_HEIGHT
        BCS :+
        STA VTEXT_HEIGHT
:
        ; WIDTH = prop3 - inward left snap, clamped to the cols right of LEFT.
        LDX #3
        JSR nz6_prop_size_cells
        SEC
        SBC nz6_marg_l
        BCS :+
        LDA #0
:
        STA VTEXT_WIDTH
        LDA #NZ6_SCREEN_COLS
        SEC
        SBC VTEXT_LEFT
        CMP VTEXT_WIDTH
        BCS :+
        STA VTEXT_WIDTH
:
        ; FLAGS from the window attributes
        LDX #14
        JSR nz6_prop_byte
        AND #$03
        STA VTEXT_FLAGS
        RTS

; Pixel-precise graphics rectangle for the target window. This intentionally
; does not snap to text cells: the gfx plane is a 320x200 pixel surface, and
; Z6 scroll_window/newline scrolling must move every pixel in the logical
; window even when the text plane can only own whole 4x4 cells inside it.
nz6_build_gfx_region_tmp_win:
        ; LEFT = prop1 - 1 (0-based pixels)
        LDX #1
        JSR nz6_prop_origin_zero
        LDA nz6_unit_lo
        STA VTEXT_GFX_LEFTL
        LDA nz6_unit_hi
        STA VTEXT_GFX_LEFTH
        ; TOP = prop0 - 1 (0-based pixels). The NDK helper validates against
        ; the 200px screen height, so an out-of-range high byte becomes $FF.
        LDX #0
        JSR nz6_prop_origin_zero
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        BRA :++
:
        LDA nz6_unit_lo
:
        STA VTEXT_GFX_TOP
        ; WIDTH = prop3 pixels (16-bit; 320 is valid)
        LDX #3
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        STA VTEXT_GFX_WIDTHL
        LDA nz6_unit_hi
        STA VTEXT_GFX_WIDTHH
        ; HEIGHT = prop2 pixels. The screen is 200px tall, so any high byte is
        ; invalid and is forced out of range for the NDK validator.
        LDX #2
        JSR nz6_read_prop_unit
        LDA nz6_unit_hi
        BEQ :+
        LDA #$FF
        BRA :++
:
        LDA nz6_unit_lo
:
        STA VTEXT_GFX_HEIGHT
        RTS

; Freshen the CURRENT window's cursor props 4/5 from the live vtext cursor
; (region-relative cells -> 1-based units). Only valid while the live region
; belongs to the current window — i.e. anywhere except inside an
; erase/clear region borrow, which always ends in nz6_apply_current_window.
nz6_sync_live_cursor:
        LDA nz6_win_current
        STA nz6_tmp_win
nz6_sync_live_cursor_tmpwin:            ; PRECONDITION: nz6_tmp_win already
                                        ; set to nz6_win_current
        LDA VTEXT_CURY
        JSR nz6_cell_to_unit
        LDX #4
        JSR nz6_write_prop_unit
        LDA VTEXT_CURX
        JSR nz6_cell_to_unit
        LDX #5
        JMP nz6_write_prop_unit

; Shared pre/post for ops that change window nz6_tmp_win's geometry
; (move_window / window_size / set_margins): when the target is the LIVE
; window, flush pending buffered text into the old geometry and freshen its
; cursor props first (prologue), then rebuild the live region from the new
; props (epilogue). Non-live targets are pure table updates.
nz6_geom_prologue:
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @rts
        JSR nz_screen_flush_word
        BRA nz6_sync_live_cursor_tmpwin
@rts:
        RTS

; Counterpart of nz6_geom_prologue: after a geometry op, re-apply the live
; vtext region ONLY when the op targeted the current window (table-only
; updates for non-live windows take effect at their next select).
nz6_geom_epilogue:
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @rts
        JMP nz6_apply_current_window
@rts:
        RTS

; --- reset --------------------------------------------------------------------

; Reset the 8-window table to the V6 boot defaults and sync the ROM text
; path. Window 0: full screen (1,1 / 200x320 units), wrap+scroll+buffer, cursor
; 1,1. Windows 1-7: origin 1,1, size 0, no attributes, cursor 1,1. All
; windows: font 1, font size 4x4 units. Current window 0.
nz6_reset_windows:
        LDX #0
@zero:
        STZ nz6_win_props,X
        INX
        BNE @zero
        LDX #7
@win:
        TXA
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        TAY
        LDA #1
        STA nz6_win_props+0,Y          ; prop 0:  y-coord = 1
        STA nz6_win_props+2,Y          ; prop 1:  x-coord = 1
        STA nz6_win_props+8,Y          ; prop 4:  y-cursor = 1
        STA nz6_win_props+10,Y         ; prop 5:  x-cursor = 1
        STA nz6_win_props+24,Y         ; prop 12: font number = 1
        LDA #4
        STA nz6_win_props+26,Y         ; prop 13: font size = $0404 units
        STA nz6_win_props+27,Y
        LDA #1
        DEX
        BPL @win
        LDA #<NZ6_SCREEN_UNIT_ROWS
        STA nz6_win_props+4            ; window 0 prop 2: y-size = 200
        LDA #>NZ6_SCREEN_UNIT_ROWS
        STA nz6_win_props+5
        LDA #<NZ6_SCREEN_UNIT_COLS
        STA nz6_win_props+6            ; window 0 prop 3: x-size = 320
        LDA #>NZ6_SCREEN_UNIT_COLS
        STA nz6_win_props+7
        LDA #%00001011
        STA nz6_win_props+28           ; window 0 prop 14: wrap+scroll+buffer
        STZ nz6_win_current
        STZ zvm_window_current         ; word-buffer gate: window 0 live
        RTS

; Dispatch id NZ6_OP_RESET: the ROM invokes this from zvm_run_until_read's
; V6 branch on every game (re)start. V6 owns the VGC palette mode for the
; whole boot: v1-5 and BASIC never come through here, and a full system
; RESET restores the power-on Nova palette before any non-V6 code runs.
nz6_op_reset:
        JSR nz6_reset_windows
        LDA #<nz6_scroll_live_composite
        LDY #>nz6_scroll_live_composite
        JSR vtext_set_scroll_hook
        JSR vgc_set_palette_ega         ; fallback for v1/no-palette packs
        LDA #VGC_MODE_TEXT_OVER_GFX     ; mode 2: text over gfx — pictures
        STA VGC_MODE                    ; show wherever cells keep the global
                                        ; background colour (mode-2 rule in
                                        ; the VGC compositor)
        LDA #VGC_TEXT_BGKEY_DEFAULT
        STA VGC_BGCOL
        LDA #VGC_GFXTRANS_DISABLE       ; gfx transparency OFF: the plane is
        STA VGC_GFXTRANS                ; the whole MCGA framebuffer, and
                                        ; opaque BLACK art (index 0) must
                                        ; render, not punch holes
        LDA #NZ6_COLOR_DEFAULT
        STA nz6_colour
        STA VTEXT_COLOR
        JSR nz6_sync_border_to_bg
        STZ zvm_text_style
        LDA #0
        JSR nz6_gfx_clear
        JSR nz6_pics_preload
        JMP nz6_apply_current_window

; --- PICS.PAK index -------------------------------------------------------------

; Load the PICS.PAK header + index into the allocated picture-index XRAM block via
; the FIO XPAGE region loader (one 8KB slice from offset 0; the host clamps
; at EOF). A missing or malformed pak is NOT an error: nz6_pics_avail stays
; 0 and the picture ops keep their honest no-pictures answers (the M2
; text-only path). Runs on every (re)start — idempotent.
nz6_pics_preload:
        STZ nz6_pics_avail
        JSR nz6_pics_set_base
        LDA #<nz6_pics_name
        STA PAGER_NAMEPTR_L
        LDA #>nz6_pics_name
        STA PAGER_NAMEPTR_H
        LDA #(nz6_pics_name_end - nz6_pics_name)
        STA PAGER_NAMELEN
        STZ PAGER_FILEL
        STZ PAGER_FILEM
        STZ PAGER_FILEH
        LDA nz6_pics_base_l
        STA PAGER_ADDRL
        LDA nz6_pics_base_m
        STA PAGER_ADDRM
        LDA nz6_pics_base_h
        STA PAGER_ADDRH
        STZ PAGER_LENL
        LDA #$20                        ; $2000: the whole 8KB index region
        STA PAGER_LENH
        STZ PAGER_TARGET                ; XRAM
        JSR pager_load_file_page
        BEQ :+
        JMP @done                       ; no pak on the image: text-only
:
        ; header v1: "NZPK" version 1, count(2 LE), release(2 LE)
        ; header v2: "NZPK" version 2, count/release, then 16 RGB triples
        LDA nz6_pics_base_m
        STA XRAM_ADDRM
        LDA nz6_pics_base_h
        STA XRAM_ADDRH
        LDX #0
@magic:
        STX XRAM_ADDRL
        JSR xram_read8
        BEQ :+
        JMP @done
:
        LDA XRAM_DATA
        CMP nz6_pics_magic,X
        BEQ :+
        JMP @done
:
        INX
        CPX #4
        BCC @magic
        JSR @next                       ; version (X=4)
        STA nz6_pics_version
        CMP #$01
        BEQ @v1
        CMP #$02
        BEQ :+
        JMP @done
:
        LDA #$39                        ; 9-byte header + 48-byte palette
        BRA @have_index
@v1:
        LDA #$09
@have_index:
        STA nz6_pics_index_l
        JSR @next                       ; count lo (X=5 from the loop)
        STA nz6_pics_count_lo
        JSR @next
        STA nz6_pics_count_hi
        JSR @next
        STA nz6_pics_release_lo
        JSR @next
        STA nz6_pics_release_hi
        ; index must fit the 8KB region: v1 count <= 545 ($0221),
        ; v2 count <= 542 ($021E) because the palette consumes 48 bytes.
        LDA nz6_pics_count_hi
        CMP #$02
        BCC @bound_ok
        BNE @done
        LDA nz6_pics_version
        CMP #$02
        BEQ @bound_v2
        LDA nz6_pics_count_lo
        CMP #$22
        BCS @done
        BRA @bound_ok
@bound_v2:
        LDA nz6_pics_count_lo
        CMP #$1F
        BCS @done
@bound_ok:
        LDA nz6_pics_count_lo
        ORA nz6_pics_count_hi
        BEQ @done                       ; empty pak: keep no-pictures mode
        LDA nz6_pics_version
        CMP #$02
        BNE :+
        LDA nz6_pics_base_l
        CLC
        ADC #9
        STA XRAM_ADDRL
        LDA nz6_pics_base_m
        ADC #0
        STA XRAM_ADDRM
        LDA nz6_pics_base_h
        ADC #0
        STA XRAM_ADDRH
        JSR vgc_set_palette_custom_xram
        BNE @done
:
        LDA #1
        STA nz6_pics_avail
@done:
        ; reflect availability in Flags1 bit 1 ("pictures available?") —
        ; header config ran before the segment loaded, so the bit is set
        ; here, where the pak truth is known. Runs on restart too.
        LDA #ZHEADER_FLAGS1
        STA zstory_addr_l
        STZ zstory_addr_m
        STZ zstory_addr_h
        JSR zstory_read8
        BNE @rts
        LDA XRAM_DATA
        AND #%11111101
        LDX nz6_pics_avail
        BEQ :+
        ORA #%00000010
:
        STA XRAM_DATA
        JSR zstory_write8
@rts:
        RTS
@next:                                  ; read header byte X, A = value
        STX XRAM_ADDRL
        JSR xram_read8
        BNE @pop_done
        INX
        LDA XRAM_DATA
        RTS
@pop_done:
        PLA                             ; abandon the header mid-read:
        PLA                             ; drop @next's return, finish @done
        BRA @done

nz6_pics_set_base:
        LDA zstory_cache_base_l
        STA nz6_pics_base_l
        LDA zstory_cache_base_m
        CLC
        ADC #ZSTORY_AUX_PICS_INDEX_OFF_M
        STA nz6_pics_base_m
        LDA zstory_cache_base_h
        ADC #$00
        STA nz6_pics_base_h
        RTS

nz6_pics_magic:
        .byte 'N', 'Z', 'P', 'K'

nz6_pics_name:
        .byte "PICS.PAK"
nz6_pics_name_end:

; --- VAR screen ops -----------------------------------------------------------

; split_window lines (VAR:10). A pure table op in V6 (Zork Zero builds its
; layout with move_window/window_size and never calls this): window 1
; becomes the top 'lines' cell rows (full width) and window 0 starts below
; it. Does NOT clear; the live region is rebuilt only when the current
; window's geometry changed as a result.
nz6_op_split:
        JSR nz_screen_flush_word
        JSR nz6_sync_live_cursor
        ; clamp the requested rows to the 50-row screen height
        LDA zvm_operand_hi
        BEQ :+
        LDA #NZ6_SCREEN_ROWS
        BRA @have
:
        LDA zvm_operand_lo
        CMP #51
        BCC @have
        LDA #NZ6_SCREEN_ROWS
@have:
        STA nz6_tmp_lines
        ; window 1: origin 1,1, y-size = lines*4, x-size = 320 units
        LDA #1
        STA nz6_tmp_win
        LDA nz6_tmp_lines
        ASL A
        ASL A
        STA nz6_unit_lo
        STZ nz6_unit_hi
        LDX #2
        JSR nz6_write_prop_unit
        JSR nz6_unit_one
        LDX #0
        JSR nz6_write_prop_unit
        LDX #1
        JSR nz6_write_prop_unit
        JSR nz6_unit_80
        LDX #3
        JSR nz6_write_prop_unit
        ; window 0: origin 1,lines*4+1, y-size = (50-lines)*4,
        ; x-size = 320 units
        STZ nz6_tmp_win
        LDA nz6_tmp_lines
        JSR nz6_cell_to_unit
        LDX #0
        JSR nz6_write_prop_unit
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC nz6_tmp_lines
        ASL A
        ASL A
        STA nz6_unit_lo
        STZ nz6_unit_hi
        LDX #2
        JSR nz6_write_prop_unit
        JSR nz6_unit_one
        LDX #1
        JSR nz6_write_prop_unit
        JSR nz6_unit_80
        LDX #3
        JSR nz6_write_prop_unit
        ; the classic unsplit idiom collapses onto window 0
        LDA nz6_tmp_lines
        BNE :+
        STZ nz6_win_current
        STZ zvm_window_current
:
        ; window 0 and 1 geometry both changed; rebuild whatever is live
        JMP nz6_apply_current_window

; set_window n (VAR:11). Flushes pending text into the old window, saves its
; cursor into the table, switches, and rebuilds the live region from the new
; window's props.
nz6_op_set_window:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        PHA
        JSR nz_screen_flush_word
        JSR nz6_sync_live_cursor
        PLA
        STA nz6_win_current
        ; ROM word-buffer gate: buffered word-wrap output is a window 0
        ; behaviour (Z attr bit 3); other windows print raw.
        TAX
        BEQ :+
        LDA #1
:
        STA zvm_window_current
        JMP nz6_apply_current_window
@rts:
        RTS

; erase_window n (VAR:13). -1: reset windows + clear all + window 0 live.
; -2: clear all, windows AND cursor untouched (the pre-clear cursor is
; synced into the table and the closing apply restores it exactly). 0-7:
; clear that window's cell rect to the background and home its cursor.
nz6_op_erase:
        JSR nz_screen_flush_word
        LDA zvm_operand_hi
        CMP #$FF
        BNE @positive
        LDA zvm_operand_lo
        CMP #$FF
        BNE :+
        JSR nz6_reset_windows           ; -1
        JSR nz6_gfx_fill_bg             ; the parchment field lives in gfx
        JMP zvm_clear_whole_screen      ; (tail-calls the V6 select -> apply)
:
        CMP #$FE
        BNE @rts
        JSR nz6_sync_live_cursor        ; -2 must leave the cursor alone
        JSR nz6_gfx_fill_bg
        JMP zvm_clear_whole_screen
@rts:
        RTS
@positive:
        ; the tail rebuilds the live region from the table — keep the
        ; current window's cursor exact before anything is borrowed
        JSR nz6_sync_live_cursor
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        ; home the window's table cursor
        JSR nz6_unit_one
        LDX #4
        JSR nz6_write_prop_unit
        LDX #5
        JSR nz6_write_prop_unit
        ; erase-owned cell rect from props 0-3; skip the fill if it lies
        ; off-grid or is empty. This intentionally differs from scroll_window:
        ; destructive erases cannot own partial edge cells shared with another
        ; pixel window.
        JSR nz6_build_inner_region_tmp_win
        LDA VTEXT_LEFT
        STA nz6_rect_left
        LDA VTEXT_TOP
        STA nz6_rect_top
        LDA VTEXT_WIDTH
        STA nz6_rect_w
        LDA VTEXT_HEIGHT
        BEQ @apply
        STA nz6_rect_h
        LDA nz6_rect_w
        BEQ @apply
        ; fill the rect with spaces in the background colour, preserving the
        ; caller-visible text colour/attr (the closing apply restores the
        ; region geometry)
        LDA VTEXT_COLOR
        PHA
        LDA VTEXT_ATTR
        PHA
        LDA nz6_rect_left
        STA VTEXT_LEFT
        LDA nz6_rect_top
        STA VTEXT_TOP
        LDA nz6_rect_w
        STA VTEXT_WIDTH
        LDA nz6_rect_h
        STA VTEXT_HEIGHT
        LDA nz6_colour                  ; erase fills with the game's colours
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        JSR vtext_clear_region
        PLA
        STA VTEXT_ATTR
        PLA
        STA VTEXT_COLOR
@apply:
        ; the rect fill borrowed the live vtext region: always rebuild it.
        ; When the erased window IS the current one its cursor home comes
        ; straight from the freshly homed table props.
        JMP nz6_apply_current_window

; set_cursor y x [window] (VAR:15). Units (= pixels), relative to the window
; origin. Row/col 0 clamps to 1: Zork Zero sends set_cursor 0,1 in its
; pre-read banner block (units are 1-based, 0 is off-grid). Some Infocom V6
; layout code also passes signed negative coordinates to address backward from
; the target window's far edge; normalize those instead of leaving the previous
; cursor live. Only a one-operand negative call is cursor visibility control.
; Only the table changes for a non-current window; the live cursor moves only
; when the target IS the current window.
nz6_op_set_cursor:
        LDA zvm_operand_count
        CMP #2
        BCS @position
        LDA zvm_operand_hi
        BMI @rts                        ; one-operand -1/-2: V6 cursor off/on
        RTS
@position:
        JSR nz_screen_flush_word
        LDA zvm_operand_count
        CMP #3
        BCC @cur
        LDX #2
        JSR nz6_window_from_operand
        BCC @rts
        BRA @have
@cur:
        LDA nz6_win_current
@have:
        STA nz6_tmp_win
        JSR nz6_cursor_y_from_operand
        LDX #4
        JSR nz6_write_prop_unit
        JSR nz6_cursor_x_from_operand
        LDX #5
        JSR nz6_write_prop_unit
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @rts
        JMP nz6_apply_current_window
@rts:
        RTS

nz6_cursor_y_from_operand:
        LDA zvm_operand_hi
        BMI @negative
        LDA zvm_operand_lo
        STA nz6_unit_lo
        LDA zvm_operand_hi
        STA nz6_unit_hi
        ORA zvm_operand_lo
        BNE @rts
        INC nz6_unit_lo                 ; row 0 -> 1
@rts:
        RTS
@negative:
        LDX #2                          ; y-size + signed y + 1
        JSR nz6_read_prop_unit
        CLC
        LDA nz6_unit_lo
        ADC zvm_operand_lo
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC zvm_operand_hi
        STA nz6_unit_hi
        INC nz6_unit_lo
        BNE :+
        INC nz6_unit_hi
:
        JMP nz6_cursor_clamp_positive

nz6_cursor_x_from_operand:
        LDA zvm_operand_hi+1
        BMI @negative
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        ORA zvm_operand_lo+1
        BNE @rts
        INC nz6_unit_lo                 ; col 0 -> 1
@rts:
        RTS
@negative:
        LDX #3                          ; x-size + signed x + 1
        JSR nz6_read_prop_unit
        CLC
        LDA nz6_unit_lo
        ADC zvm_operand_lo+1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC zvm_operand_hi+1
        STA nz6_unit_hi
        INC nz6_unit_lo
        BNE :+
        INC nz6_unit_hi
:
        ; fall through
nz6_cursor_clamp_positive:
        LDA nz6_unit_hi
        BMI @one                        ; still before the window
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BNE @rts
@one:
        JMP nz6_unit_one
@rts:
        RTS

; get_cursor array (VAR:16). Writes the CURRENT window's cursor as two
; words (y then x, V6 units/pixels, window-relative, exactly as stored in
; props 4/5) into Z-memory. Pending buffered text is flushed first — it
; will occupy cells from the reported position onward.
nz6_op_get_cursor:
        JSR nz_screen_flush_word
        JSR nz6_sync_live_cursor        ; leaves nz6_tmp_win = current window
        LDX #4
        JSR nz6_read_prop_unit
        LDA zvm_operand_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA nz6_unit_lo
        STA zstory_word_lo
        LDA nz6_unit_hi
        STA zstory_word_hi
        JSR zstory_write16
        LDA nz6_win_current
        STA nz6_tmp_win
        LDX #5
        JSR nz6_read_prop_unit
        LDA zvm_operand_lo
        CLC
        ADC #2
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC #0
        STA zstory_addr_m
        STZ zstory_addr_h
        LDA nz6_unit_lo
        STA zstory_word_lo
        LDA nz6_unit_hi
        STA zstory_word_hi
        JMP zstory_write16

; set_colour fg bg [window] (2OP:27). Records the raw colour data in prop 11
; ((bg<<8)|fg) and, when the target is the live window, maps the Z colour
; codes onto EGA-compatible indices and drives the live text colour: codes 2-9 are the
; spec table, 1 restores the default nibble, 0 and anything out of range
; (including the V6 -1 "pixel under cursor" idiom) keep the current nibble.
; An absent or out-of-range window operand (including the -3 "current
; window" idiom) targets the current window.
nz6_op_set_colour:
        JSR nz_screen_flush_word        ; pending text keeps the OLD colour
        LDA nz6_win_current
        STA nz6_tmp_win
        LDA zvm_operand_count
        CMP #3
        BCC :+
        LDX #2
        JSR nz6_window_from_operand
        BCC :+
        STA nz6_tmp_win
:
        LDA zvm_operand_lo              ; fg -> low byte
        STA nz6_unit_lo
        LDA zvm_operand_lo+1            ; bg -> high byte
        STA nz6_unit_hi
        LDX #11
        JSR nz6_write_prop_unit
        LDA nz6_tmp_win                 ; live colour only for the current window
        CMP nz6_win_current
        BNE @rts
        ; fg nibble
        LDX #0
        JSR nz6_map_colour_operand
        BCC :+
        STA nz6_clr_tmp
        LDA nz6_colour
        AND #$F0
        ORA nz6_clr_tmp
        STA nz6_colour
:
        ; bg nibble
        LDX #1
        JSR nz6_map_colour_operand
        BCC :+
        ASL A
        ASL A
        ASL A
        ASL A
        STA nz6_clr_tmp
        LDA nz6_colour
        AND #$0F
        ORA nz6_clr_tmp
        STA nz6_colour
:
        JMP nz6_apply_colour_style
@rts:
        RTS

; Z colour code in operand X -> palette index in A with C=1, or C=0 for "keep
; the current nibble" (code 0 and anything unmapped). Code 1 = default:
; fg white ($F), bg black ($0) — X selects which. Code -1 = the colour of
; the gfx pixel under the cursor (spec 8.3.1 V6, the Frotz os_peek_colour
; behaviour Zork Zero's banner is tuned to: text blends into the ribbon
; art instead of punching solid boxes).
nz6_map_colour_operand:
        LDA zvm_operand_hi,X
        CMP #$FF
        BNE :+
        LDA zvm_operand_lo,X
        CMP #$FF
        BNE @keep                       ; other negatives: keep
        JMP nz6_sample_cursor_pixel     ; -1: A = pixel colour, C=1
:
        LDA zvm_operand_hi,X
        BNE @keep                       ; large: keep
        LDA zvm_operand_lo,X
        BEQ @keep                       ; 0 = current colour
        CMP #1
        BEQ @default
        CMP #10
        BCS @keep                       ; 10+ unmapped: keep
        TAY
        LDA nz6_colour_table-2,Y        ; codes 2-9
        SEC
        RTS
@default:
        CPX #0
        BNE :+
        LDA #(NZ6_COLOR_DEFAULT & $0F)  ; default fg
        SEC
        RTS
:
        LDA #(NZ6_COLOR_DEFAULT >> 4)   ; default bg
        SEC
        RTS
@keep:
        CLC
        RTS

; set_text_style n (VAR:17), V6 path (dispatch id NZ6_OP_TEXT_STYLE). Styles
; accumulate like the ROM handler (0 resets). Reverse is recorded as a
; text-plane attribute so the VGC keeps reverse backgrounds opaque in
; text-over-gfx mode. Bold is intentionally not implemented by changing the
; palette index: V6 asset packs can load custom palettes, where fg|8 is not
; guaranteed to be a brighter readable version of the same colour.
nz6_op_text_style:
        JSR nz_screen_flush_word
        LDA zvm_operand_lo
        BNE @accumulate
        STZ zvm_text_style
        BRA nz6_apply_colour_style
@accumulate:
        ORA zvm_text_style
        STA zvm_text_style
        ; FALL THROUGH to nz6_apply_colour_style

; VTEXT_COLOR := nz6_colour. VTEXT_ATTR gets VTXT_ATTR_REV for bit 0 reverse.
; Bold/italic/fixed accumulate in zvm_text_style but render as roman until the
; platform has real font/style support for them.
nz6_apply_colour_style:
        LDA nz6_colour
        STA nz6_clr_tmp
        LDA zvm_text_style
        AND #$01
        BEQ @reverse_off
        LDA VTEXT_ATTR
        ORA #VTXT_ATTR_REV
        BRA @attr_ready
@reverse_off:
        LDA VTEXT_ATTR
        AND #$FD
@attr_ready:
        STA VTEXT_ATTR
@store:
        LDA nz6_clr_tmp
        STA VTEXT_COLOR
        RTS

; Keep the physical display border aligned with the V6 logical background.
; V6 paper colour lives in nz6_colour and per-cell colour bytes; VGC_BGCOL is
; only the mode-2 text-background key, so the border must be synced explicitly.
nz6_sync_border_to_bg:
        LDA nz6_colour
        LSR A
        LSR A
        LSR A
        LSR A
        STA VGC_BORDER
        RTS

; Legacy ROM pre-newline-scroll hook (NZ6_OP_PRE_NEWLINE_SCROLL). Mixed
; scrolling now belongs to the VTEXT automatic-scroll hook installed by
; nz6_op_reset, so the old ahead-of-newline dispatch is intentionally inert.
nz6_op_pre_newline_scroll:
        STZ nz_lf_scrolled
        RTS

; VTEXT automatic-scroll hook for V6. VGC text and gfx live in separate
; planes, but V6's MCGA model treats them as one surface. VTEXT reaches this
; hook with the current text-output rect, which may be margin-shrunk. Scroll the
; full current window instead, then restore the margin rect before vtext_newline
; clamps the live cursor. NovaZ supplies two rectangles to the NDK: the exact
; pixel window for gfx, and the snapped text-cell window for text. That keeps
; non-cell-aligned Z6 chrome from being overwritten without leaving stale gfx
; pixels under transparent text cells.
nz6_scroll_live_composite:
        LDA #1
        STA nz6_tmp_lines
        LDA VTEXT_LEFT
        STA nz6_rect_left
        LDA VTEXT_TOP
        STA nz6_rect_top
        LDA VTEXT_WIDTH
        STA nz6_rect_w
        LDA VTEXT_HEIGHT
        STA nz6_rect_h
        LDA VTEXT_FLAGS
        STA nz6_stk_lo
        LDA nz6_win_current
        STA nz6_tmp_win
        JSR nz6_build_full_region_tmp_win
        JSR nz6_clear_scroll_excluded_top_rows
        LDA nz6_tmp_lines
        JSR nz6_scroll_live_rows_composite
        PHA
        LDA nz6_rect_left
        STA VTEXT_LEFT
        LDA nz6_rect_top
        STA VTEXT_TOP
        LDA nz6_rect_w
        STA VTEXT_WIDTH
        LDA nz6_rect_h
        STA VTEXT_HEIGHT
        LDA nz6_stk_lo
        STA VTEXT_FLAGS
        PLA
        RTS

; The text renderer can place existing glyphs in the floor-aligned top cell,
; but composite scrolling uses only complete cells inside the pixel window so
; new text does not overwrite adjacent chrome. Clear the excluded leading rows
; before every scroll, or those old glyphs become pinned above the scrollable
; transcript.
nz6_clear_scroll_excluded_top_rows:
        LDX #0
        JSR nz6_prop_top_cell
        CMP VTEXT_TOP
        BCC :+
        RTS
:
        STA nz6_marg_l                  ; clear top
        LDA VTEXT_TOP
        SEC
        SBC nz6_marg_l
        BEQ @rts
        STA nz6_marg_r                  ; clear height
        LDA VTEXT_LEFT
        PHA
        LDA VTEXT_TOP
        PHA
        LDA VTEXT_WIDTH
        PHA
        LDA VTEXT_HEIGHT
        PHA
        LDA VTEXT_COLOR
        PHA
        LDA VTEXT_ATTR
        PHA
        LDA nz6_marg_l
        STA VTEXT_TOP
        LDA nz6_marg_r
        STA VTEXT_HEIGHT
        LDA #NZ6_COLOR_DEFAULT          ; bg 0: reveal graphics/chrome
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        JSR vtext_clear_region
        PLA
        STA VTEXT_ATTR
        PLA
        STA VTEXT_COLOR
        PLA
        STA VTEXT_HEIGHT
        PLA
        STA VTEXT_WIDTH
        PLA
        STA VTEXT_TOP
        PLA
        STA VTEXT_LEFT
@rts:
        RTS

nz6_scroll_live_rows_composite:
        STA nz6_tmp_lines
        JSR nz6_build_gfx_region_tmp_win
        LDA nz6_colour
        LSR A
        LSR A
        LSR A
        LSR A
        TAX
        LDA nz6_tmp_lines
        ASL A
        ASL A
        JSR vtext_scroll_gfx_pixels_up
        BNE @done
        LDA nz6_tmp_lines
        BEQ @ok
        CMP VTEXT_HEIGHT
        BCC @scroll_text
        JMP vtext_clear_region
@scroll_text:
        LDX nz6_tmp_lines
@loop:
        PHX
        JSR vtext_scroll_up
        TAY
        PLX
        TYA
        BNE @done
        DEX
        BNE @loop
@ok:
        LDA #0
@done:
        RTS

; ROM newline hook (NZ6_OP_NEWLINE): the V6 carriage-return interrupt
; (YZIP "CRCNT/CRFUNC", window props 9/8). A non-zero countdown decrements
; on every newline printed to the live window; reaching 0 calls the prop-8
; routine with no arguments — AFTER the newline went out, the Frotz r393
; ordering Zork Zero's drop-cap margin release (RESET-MARGIN) was tuned
; against. $FFFF decrements harmlessly. While the countdown sits at 0 the
; feature is off, which also keeps a printing CR routine from recursing.
; NOTE: runs inside the print path — clobbers nz6_tmp_win (reset to the
; current window) and nz6_unit, and the fired routine may run arbitrary
; Z-code (audited callers: nz_screen_flush_word sites set their state
; after flushing).
nz6_op_cr_newline:
        LDA nz6_win_current
        STA nz6_tmp_win
        LDX #9
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BEQ @rts                        ; 0 = disarmed
        LDA nz6_unit_lo
        BNE :+
        DEC nz6_unit_hi
:
        DEC nz6_unit_lo
        LDX #9
        JSR nz6_write_prop_unit
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BNE @rts                        ; still counting
        LDX #8
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BEQ @rts                        ; no routine armed
        ; The fire happens MID-FLUSH: the word that triggered the wrap is
        ; still in the ROM's word buffer. The callback must see an empty
        ; buffer so its geometry ops cannot flush the outer word at the old
        ; cursor, but nested printing still reuses nz_word_buf. Preserve the
        ; pending bytes as well as the length before running arbitrary Z-code.
        LDA nz_word_len
        STA nz6_saved_word_len
        BEQ @word_saved
        LDX #0
@save_word:
        CPX nz6_saved_word_len
        BCS @word_saved
        LDA nz_word_buf,X
        STA nz6_saved_word_buf,X
        INX
        BRA @save_word
@word_saved:
        STZ nz_word_len
        LDA nz6_unit_lo
        LDX nz6_unit_hi
        JSR nz_call_z_routine           ; packed routine, no args
        LDA nz6_saved_word_len
        BEQ @word_restored
        LDX #0
@restore_word:
        CPX nz6_saved_word_len
        BCS @word_restored
        LDA nz6_saved_word_buf,X
        STA nz_word_buf,X
        INX
        BRA @restore_word
@word_restored:
        LDA nz6_saved_word_len
        STA nz_word_len
@rts:
        RTS

; A := the gfx pixel colour under the live cursor (the cursor cell's
; top-left pixel), C=1. Uses the VGC MEMREAD command (P0=space, P1/P2=addr,
; P3=result); pixel addr = cellrow*4*320 + cellcol*4, same 16-bit shape as
; the draw destination (row*1280 lands wholly in the high byte as row*5).
nz6_sample_cursor_pixel:
        LDA VTEXT_LEFT
        CLC
        ADC VTEXT_CURX                  ; abs cell col
        STZ VGC_P1
        STZ VGC_P2
        ASL A
        ROL VGC_P2
        ASL A
        ROL VGC_P2
        STA VGC_P1                      ; col*4 (16-bit across P1/P2)
        LDA VTEXT_TOP
        CLC
        ADC VTEXT_CURY
        STA nz6_clr_tmp
        ASL A
        ASL A
        CLC
        ADC nz6_clr_tmp                 ; row*5 (max 245)
        CLC
        ADC VGC_P2
        STA VGC_P2                      ; + row*1280 in the high byte
        LDA #$03                        ; MemSpaceGfx
        STA VGC_P0
        STZ VGC_P4
        LDA #VCMD_MEMREAD
        STA VGC_CMD
@wait:
        LDA VGC_CMD
        AND #$01
        BNE @wait
        LDA VGC_P3
        AND #$0F
        SEC
        RTS

; EGA-compatible indices for Z-machine colour codes 2-9 (spec section 8.3.1):
; black red green yellow blue magenta cyan white.
nz6_colour_table:
        .byte $00, $04, $02, $0E, $01, $05, $03, $0F

; --- EXT window ops -----------------------------------------------------------

; set_margins left right [window] (EXT:8). Stored in props 6/7; margins
; shrink the writable region (LEFT moves right, WIDTH loses both margins),
; so a live target rebuilds the region.
nz6_ext_set_margins:
        LDA nz6_win_current
        STA nz6_tmp_win
        LDA zvm_operand_count
        CMP #3
        BCC :+
        LDX #2
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
:
        JSR nz6_geom_prologue
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @write_props
        LDA VTEXT_LEFT
        STA nz6_rect_left               ; old writable left edge
        CLC
        ADC VTEXT_WIDTH
        STA nz6_rect_w                  ; old writable right edge
        LDA VTEXT_TOP
        CLC
        ADC VTEXT_CURY
        CLC
        ADC #1
        STA nz6_rect_top                ; release starts below current row
@write_props:
        LDA zvm_operand_lo
        STA nz6_unit_lo
        LDA zvm_operand_hi
        STA nz6_unit_hi
        LDX #6
        JSR nz6_write_prop_unit
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        LDX #7
        JSR nz6_write_prop_unit
        JSR nz6_restore_released_margin_spaces
        JMP nz6_geom_epilogue
@rts:
        RTS

; When a V6 flow picture releases a temporary text margin, cells in the newly
; writable columns are ordinary background again. They must not keep the
; transparent picture style or stale icon pixels will show through later blank
; text. Restyle only space cells below the current output row; rows at/above
; the cursor may still belong to the inline picture/text already printed.
nz6_restore_released_margin_spaces:
        LDA nz6_tmp_win
        CMP nz6_win_current
        BEQ :+
        RTS
:
        JSR nz6_build_region_tmp_win    ; new writable rect
        LDA VTEXT_LEFT
        STA nz6_marg_l                  ; new left
        CLC
        ADC VTEXT_WIDTH
        STA nz6_marg_r                  ; new right edge
        LDA VTEXT_TOP
        STA nz6_stk_lo                  ; new top
        CLC
        ADC VTEXT_HEIGHT
        STA nz6_stk_hi                  ; new bottom edge

        ; Clamp release top into the new window.
        LDA nz6_rect_top
        CMP nz6_stk_lo
        BCS :+
        LDA nz6_stk_lo
:
        STA nz6_rect_top
        CMP nz6_stk_hi
        BCC :+
        RTS
:
        LDA nz6_stk_hi
        SEC
        SBC nz6_rect_top
        STA nz6_rect_h
        BEQ @done

        ; Left margin shrank: [new_left, old_left).
        LDA nz6_marg_l
        CMP nz6_rect_left
        BCS @right
        STA nz6_clr_tmp
        LDA nz6_rect_left
        SEC
        SBC nz6_clr_tmp
        BEQ @right
        STA nz6_tmp_lines
        LDA nz6_clr_tmp
        JSR nz6_restore_space_rect

@right:
        ; Right margin shrank: [old_right, new_right).
        LDA nz6_rect_w
        CMP nz6_marg_r
        BCS @done
        LDA nz6_marg_r
        SEC
        SBC nz6_rect_w
        BEQ @done
        STA nz6_tmp_lines
        LDA nz6_rect_w
        JSR nz6_restore_space_rect
@done:
        RTS

; A = left, nz6_tmp_lines = width, nz6_rect_top/h = absolute row/height.
nz6_restore_space_rect:
        STA VTEXT_LEFT
        LDA nz6_rect_top
        STA VTEXT_TOP
        LDA nz6_tmp_lines
        STA VTEXT_WIDTH
        LDA nz6_rect_h
        STA VTEXT_HEIGHT
        LDA nz6_colour                  ; clear stale gfx pixels under cells
        LSR A
        LSR A
        LSR A
        LSR A
        TAX
        JSR vtext_fill_gfx_region
        LDA VTEXT_COLOR
        PHA
        LDA VTEXT_ATTR
        PHA
        LDA nz6_colour                  ; opaque current background
        STA VTEXT_COLOR
        STZ VTEXT_ATTR
        JSR vtext_expose_gfx_spaces_region
        PLA
        STA VTEXT_ATTR
        PLA
        STA VTEXT_COLOR
        RTS

; Shared tail for move_window/window_size: operand1 -> prop X, operand2 ->
; prop X+1 of window nz6_tmp_win.
nz6_yx_from_ops12:
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        JSR nz6_write_prop_unit
        INX
        LDA zvm_operand_lo+2
        STA nz6_unit_lo
        LDA zvm_operand_hi+2
        STA nz6_unit_hi
        JMP nz6_write_prop_unit

; move_window window y x (EXT:16): position -> props 0/1. Zork Zero moves
; the LIVE window 0 at boot (capture seq 15), so a live target rebuilds the
; region.
nz6_ext_move_window:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        JSR nz6_geom_prologue
        LDX #0
        JSR nz6_yx_from_ops12
        JMP nz6_geom_epilogue
@rts:
        RTS

; window_size window y x (EXT:17): size -> props 2/3. Same live-target
; rebuild as move_window (capture seq 16).
nz6_ext_window_size:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        JSR nz6_geom_prologue
        LDX #2
        JSR nz6_yx_from_ops12
        JMP nz6_geom_epilogue
@rts:
        RTS

; scroll_window window pixels (EXT:20). A positive amount scrolls the
; window's text up/forward (Z-spec 1.1); NovaZ's text plane is 4px-high
; cells, so the V6 unit amount is rounded up at the VTEXT boundary. The
; vacated bottom rows come back blank in the live text colour (M2's
; single-colour model — per-window colour rendering, prop 11, is out of M2
; scope). Works on ANY window: the target's full rect, not its current text
; margins, is borrowed into the live vtext state, scrolled, and the current
; window's region rebuilt afterwards. The target's cursor props do NOT move.
; Amounts >= the window height degenerate to a whole-rect clear. Negative
; amounts (scroll down/backward): vtext has no descending row blit, so fail
; loudly through the ROM's unsupported-opcode path rather than silently
; mis-rendering.
nz6_ext_scroll_window:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        PHA
        ; the live region is about to be borrowed: flush pending buffered
        ; text into it and save the current window's cursor first
        JSR nz_screen_flush_word
        JSR nz6_sync_live_cursor        ; clobbers nz6_tmp_win (:= current)
        PLA
        STA nz6_tmp_win
        LDA zvm_operand_hi+1
        BMI @negative
        ORA zvm_operand_lo+1
        BEQ @restore                    ; amount 0: nothing to scroll
        JSR nz6_build_full_region_tmp_win ; target rect -> live vtext state
        JSR nz6_clear_scroll_excluded_top_rows
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        JSR nz6_unit_size_to_cells
        STA nz6_tmp_lines
        CMP VTEXT_HEIGHT
        BCS @clear                      ; >= height: ditto (height 0 too)
        JSR nz6_scroll_live_rows_composite
        BNE @restore
@restore:
        JMP nz6_apply_current_window
@clear:
        LDA VTEXT_HEIGHT
        BEQ :+
        JSR nz6_scroll_live_rows_composite ; rows >= height clears the rect
:
        BRA @restore
@negative:
        JMP zvm_unsupported
@rts:
        RTS

; window_style window flags op (EXT:18) against prop 14:
; op 0 = set, 1 = or, 2 = and-not, 3 = xor. A missing op operand means 0.
nz6_ext_window_style:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        LDX #14
        JSR nz6_read_prop_unit          ; current attributes -> nz6_unit
        LDA zvm_operand_count
        CMP #3
        BCC @op_set
        LDA zvm_operand_lo+2
        AND #$03
        BEQ @op_set
        CMP #1
        BEQ @op_or
        CMP #2
        BEQ @op_clear
        LDA nz6_unit_lo                 ; op 3: xor
        EOR zvm_operand_lo+1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        EOR zvm_operand_hi+1
        STA nz6_unit_hi
        BRA @store
@op_or:
        LDA nz6_unit_lo
        ORA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ORA zvm_operand_hi+1
        STA nz6_unit_hi
        BRA @store
@op_clear:
        LDA zvm_operand_lo+1
        EOR #$FF
        AND nz6_unit_lo
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        EOR #$FF
        AND nz6_unit_hi
        STA nz6_unit_hi
        BRA @store
@op_set:
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
@store:
        LDX #14
        JMP nz6_write_prop_unit
@rts:
        RTS

; get_wind_prop window prop (EXT:19, store). Out-of-range window or prop
; stores 0. The store byte MUST be consumed on every path. Cursor props 4/5
; of the LIVE window are freshened (flush + sync) before the read — Zork
; Zero measures prop 5 (x-cursor) every turn to lay out its input line.
nz6_ext_get_wind_prop:
        STZ zvm_value_lo
        STZ zvm_value_hi
        LDX #0
        JSR nz6_window_from_operand
        BCC @store
        STA nz6_tmp_win
        LDA zvm_operand_hi+1
        BNE @store
        LDA zvm_operand_lo+1
        CMP #16
        BCS @store
        TAX
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @read
        CPX #4
        BEQ @fresh
        CPX #5
        BNE @read
@fresh:
        PHX
        JSR nz_screen_flush_word
        JSR nz6_sync_live_cursor        ; re-stores nz6_tmp_win = current
        PLX
@read:
        JSR nz6_read_prop_unit
        LDA nz6_unit_lo
        STA zvm_value_lo
        LDA nz6_unit_hi
        STA zvm_value_hi
@store:
        JSR zvm_fetch
        JMP zvm_set_var

; put_wind_prop window prop value (EXT:25). M1 writes any prop 0-15 (games
; write what they want); out-of-range writes are ignored. Takes effect on
; the live cursor only when the window is next selected.
nz6_ext_put_wind_prop:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        LDA zvm_operand_hi+1
        BNE @rts
        LDA zvm_operand_lo+1
        CMP #16
        BCS @rts
        TAX
        LDA zvm_operand_lo+2
        STA nz6_unit_lo
        LDA zvm_operand_hi+2
        STA nz6_unit_hi
@write:
        JMP nz6_write_prop_unit
@rts:
        RTS

; --- User stacks (EXT:24 push_stack, EXT:21 pop_stack) -------------------------
; A user stack is a Z-memory word table whose word 0 holds the number of FREE
; slots; pushed values fill the table DOWNWARD from its far end (the first
; push on a fresh N-slot stack lands at word index N).

; zstory_addr := user-stack table base (operand 1). Clobbers A.
nz6_ustack_addr:
        LDA zvm_operand_lo+1
        STA zstory_addr_l
        LDA zvm_operand_hi+1
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

; push_stack value stack ?(label) (EXT:24): write value at word[free],
; decrement word 0, branch TRUE. With no free slots: write NOTHING (not even
; the count) and branch FALSE.
nz6_ext_push_stack:
        JSR nz6_ustack_addr
        JSR zstory_read16               ; word 0 -> zstory_word (free slots)
        LDA zstory_word_lo
        ORA zstory_word_hi
        BEQ @full
        LDA zstory_word_lo              ; keep the count for the decrement
        STA nz6_stk_lo
        LDA zstory_word_hi
        STA nz6_stk_hi
        ; zstory_addr := table + 2*free (the slot for the new value)
        LDA zstory_word_lo
        ASL A
        STA nz6_unit_lo
        LDA zstory_word_hi
        ROL A
        STA nz6_unit_hi
        STZ zstory_addr_h
        ROL zstory_addr_h               ; bit 16 of 2*free
        CLC
        LDA zvm_operand_lo+1
        ADC nz6_unit_lo
        STA zstory_addr_l
        LDA zvm_operand_hi+1
        ADC nz6_unit_hi
        STA zstory_addr_m
        LDA zstory_addr_h
        ADC #0
        STA zstory_addr_h
        LDA zvm_operand_lo              ; value -> word[free]
        STA zstory_word_lo
        LDA zvm_operand_hi
        STA zstory_word_hi
        JSR zstory_write16
        ; word 0 := free - 1
        JSR nz6_ustack_addr
        LDA nz6_stk_lo
        BNE :+
        DEC nz6_stk_hi
:
        DEC nz6_stk_lo
        LDA nz6_stk_lo
        STA zstory_word_lo
        LDA nz6_stk_hi
        STA zstory_word_hi
        JSR zstory_write16
        JMP zvm_branch_true
@full:
        JMP zvm_branch_false

; pop_stack items stack (EXT:21): throw away 'items' entries by adding them
; back to the free-slot count (word 0). NO store, NO branch; the discarded
; values stay in memory — the spec only requires the count to move. The
; stack-less/zero-stack form pops the GAME stack instead; that form is
; deferred to M2 (the V6 'pull' opcode itself IS implemented — see
; nz6_var_pull), so it fails loudly
; through the ROM's unsupported-opcode path rather than silently no-opping.
nz6_ext_pop_stack:
        LDA zvm_operand_count
        CMP #2
        BCC @game_stack
        LDA zvm_operand_lo+1
        ORA zvm_operand_hi+1
        BEQ @game_stack
        JSR nz6_ustack_addr
        JSR zstory_read16               ; word 0 -> zstory_word
        JSR nz6_ustack_addr             ; read16 advanced the address
        CLC
        LDA zstory_word_lo
        ADC zvm_operand_lo
        STA zstory_word_lo
        LDA zstory_word_hi
        ADC zvm_operand_hi
        STA zstory_word_hi
        JMP zstory_write16
@game_stack:
        JMP zvm_unsupported

; pull stack -> (result) (VAR:9, routed here only for V6). The V6 form is a
; STORE op. With a nonzero user-stack operand it is the exact reverse of
; push_stack: word 0 (the FREE-slot count) is incremented and the value is
; read back from word[new count]. With the operand absent (types $FF) or
; zero, it pops the game main stack, exactly like the classic op — but the
; result still goes through the store byte. NOTE: pull's user-stack operand
; is its FIRST operand (push_stack's is its second), so nz6_ustack_addr is
; not reusable here.
nz6_var_pull:
        LDA zvm_operand_count
        BEQ @game
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BEQ @game
        ; word 0 -> free count
        JSR @stack_addr
        JSR zstory_read16
        INC zstory_word_lo
        BNE :+
        INC zstory_word_hi
:
        LDA zstory_word_lo              ; keep free+1 for the slot address
        STA nz6_stk_lo
        LDA zstory_word_hi
        STA nz6_stk_hi
        JSR @stack_addr                 ; read16 advanced the address
        JSR zstory_write16              ; word 0 := free + 1
        ; zstory_addr := table + 2*(free+1) — the slot holding the value
        LDA nz6_stk_lo
        ASL A
        STA nz6_unit_lo
        LDA nz6_stk_hi
        ROL A
        STA nz6_unit_hi
        STZ zstory_addr_h
        ROL zstory_addr_h               ; bit 16 of 2*(free+1)
        CLC
        LDA zvm_operand_lo
        ADC nz6_unit_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        ADC nz6_unit_hi
        STA zstory_addr_m
        LDA zstory_addr_h
        ADC #0
        STA zstory_addr_h
        JSR zstory_read16
        LDA zstory_word_lo
        STA zvm_value_lo
        LDA zstory_word_hi
        STA zvm_value_hi
        BRA @store
@game:
        JSR zvm_stack_pop               ; value -> zvm_value_lo/hi
@store:
        JSR zvm_fetch                   ; store byte -> A
        JMP zvm_set_var
@stack_addr:                            ; zstory_addr := operand 0, pull's
                                        ; user-stack table base
        LDA zvm_operand_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        RTS

; --- No-capability graphics/mouse stubs ---------------------------------------

; Write a zero word at zstory_addr (zstory_write16 advances the address by
; two, so calls chain across consecutive table words).
nz6_write_zero_word:
        STZ zstory_word_lo
        STZ zstory_word_hi
        JMP zstory_write16

; picture_data N table ?(label) (EXT:6), answered from the XRAM-resident
; PICS.PAK index. N=0: word 0 := picture count, word 1 := release; branch
; TRUE iff any pictures (no pak: 0/0 and branch false — the M2 behavior).
; N>0: index lookup — found: word 0 := height, word 1 := width, both in
; V6 units (gfx pixels), branch TRUE; unknown: nothing written, branch FALSE.
nz6_ext_picture_data:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BNE @specific
        JSR nz6_ustack_addr             ; table = operand 1, same idiom
        LDA nz6_pics_count_hi
        STA zstory_word_hi
        LDA nz6_pics_count_lo
        STA zstory_word_lo
        JSR zstory_write16              ; word 0: count (addr advances)
        LDA nz6_pics_release_hi
        STA zstory_word_hi
        LDA nz6_pics_release_lo
        STA zstory_word_lo
        JSR zstory_write16              ; word 1: release
        LDA nz6_pics_avail
        BEQ @false
        JMP zvm_branch_true
@specific:
        JSR nz6_pics_find
        BCC @false
        JSR nz6_ustack_addr
        ; The story floors (reported_height - 1) to text rows when setting
        ; its temporary margin-release countdown. For small framed flow icons,
        ; report the 4px-aligned flow box plus the inclusive endpoint, so a
        ; 21px icon reserves six 4px rows while draw_picture still uses the
        ; real bitmap height.
        LDA nz6_pic_flags
        AND #NZ6_PIC_FLAG_FLOW_ICON
        BEQ @raw_height
        LDA nz6_pic_w_hi
        ORA nz6_pic_h_hi
        BNE @raw_height
        LDA #32
        CMP nz6_pic_w_lo
        BCC @raw_height
        CMP nz6_pic_h_lo
        BCC @raw_height
        LDA nz6_pic_h_lo
        CLC
        ADC #3
        AND #$FC
        CLC
        ADC #1
        STA zstory_word_lo
        STZ zstory_word_hi
        BRA @write_height
@raw_height:
        LDA nz6_pic_h_lo                ; word 0: height units
        STA zstory_word_lo
        LDA nz6_pic_h_hi
        STA zstory_word_hi
@write_height:
        JSR zstory_write16
        LDA nz6_pic_w_lo                ; word 1: width units
        STA zstory_word_lo
        LDA nz6_pic_w_hi
        STA zstory_word_hi
        JSR zstory_write16
        JMP zvm_branch_true
@false:
        JMP zvm_branch_false

; Look up picture N (operand 0) in the index: C=1 found with the entry
; fields cached in nz6_pic_*, C=0 unknown picture or no pak. The index is
; sorted by z_number ascending, so the scan stops early at the first entry
; past N.
nz6_pics_find:
        LDA nz6_pics_avail
        BNE :+
        CLC
        RTS
:
        LDA nz6_pics_base_l             ; cursor := first entry
        CLC
        ADC nz6_pics_index_l
        STA nz6_pics_cur_l
        LDA nz6_pics_base_m
        ADC #$00
        STA nz6_pics_cur_m
        LDA nz6_pics_base_h
        ADC #$00
        STA nz6_pics_cur_h
        LDA nz6_pics_count_lo
        STA nz6_pics_rem_lo
        LDA nz6_pics_count_hi
        STA nz6_pics_rem_hi
@loop:
        LDA nz6_pics_rem_lo
        ORA nz6_pics_rem_hi
        BEQ @notfound
        JSR nz6_pics_next_byte          ; entry z_number lo
        STA nz6_pic_tmp
        JSR nz6_pics_next_byte          ; entry z_number hi
        CMP zvm_operand_hi
        BNE @hi_differs
        LDA nz6_pic_tmp
        CMP zvm_operand_lo
        BEQ @found
        BCS @notfound                   ; entry > N: sorted, stop
        BRA @skip
@hi_differs:
        BCS @notfound                   ; entry hi > N hi: sorted, stop
@skip:
        CLC                             ; skip the 13 remaining entry bytes
        LDA nz6_pics_cur_l
        ADC #13
        STA nz6_pics_cur_l
        BCC :+
        INC nz6_pics_cur_m
        BNE :+
        INC nz6_pics_cur_h
:
        LDA nz6_pics_rem_lo
        BNE :+
        DEC nz6_pics_rem_hi
:
        DEC nz6_pics_rem_lo
        BRA @loop
@found:
        JSR nz6_pics_next_byte
        STA nz6_pic_w_lo
        JSR nz6_pics_next_byte
        STA nz6_pic_w_hi
        JSR nz6_pics_next_byte
        STA nz6_pic_h_lo
        JSR nz6_pics_next_byte
        STA nz6_pic_h_hi
        JSR nz6_pics_next_byte
        STA nz6_pic_flags
        JSR nz6_pics_next_byte
        STA nz6_pic_off_l
        JSR nz6_pics_next_byte
        STA nz6_pic_off_m
        JSR nz6_pics_next_byte
        STA nz6_pic_off_h
        JSR nz6_pics_next_byte          ; offset byte 3: pak < 16MB, ignore
        JSR nz6_pics_next_byte
        STA nz6_pic_len_lo
        JSR nz6_pics_next_byte
        STA nz6_pic_len_hi
        ; len bytes 2-3 ignored: bitmaps cap at 32000 (320x200 4bpp)
        SEC
        RTS
@notfound:
        CLC
        RTS

; --- draw_picture / erase_picture ---------------------------------------------
;
; Blit path: FIO XPAGE streams the bitmap region from PICS.PAK into the XRAM
; bounce buffer, then the blitter unpacks row-packed 4bpp bytes to the gfx
; layer. Transparent pictures set BLT_CKEY to the transparent index; opaque
; pictures set it above 15 to disable skips.
; Coordinates are window-relative 1-based V6 units (= gfx pixels). Rects clip
; at 320x200; fully off-screen draws are clean no-ops. Text invalidation
; snaps the covered pixel rect back to 4x4 VTEXT cells after the bitmap lands.

; draw_picture N [y x] (EXT:5). Missing/unknown N and Rect placeholders
; (len 0) draw nothing.
nz6_ext_draw_picture:
        JSR nz6_pics_find
        BCS :+
@rts:
        RTS
:
        LDA nz6_pic_len_lo
        ORA nz6_pic_len_hi
        BEQ @rts                        ; Rect placeholder: invisible
        JSR nz6_pic_rect_setup
        BCC @rts                        ; off-screen left/top
        ; Larger framed flow pictures (Zork Zero's drop caps) align to the
        ; top of the containing text cell. This keeps the picture inside the
        ; margin countdown rows the story already computes.
        LDA nz6_pic_flags
        AND #NZ6_PIC_FLAG_FLOW_CELLTOP
        BEQ @flow_icon
        LDA nz6_pic_w_hi
        ORA nz6_pic_h_hi
        BNE @flow_icon
        LDA #64
        CMP nz6_pic_w_lo
        BCC @flow_icon
        CMP nz6_pic_h_lo
        BCC @flow_icon
        LDA nz6_blt_py
        AND #$03
        BEQ @flow_icon
        STA nz6_pic_tmp
        INC nz6_pic_tmp                 ; 35px drop caps sit in 36px flow slots:
                                        ; bias the spare pixel to the bottom.
        SEC
        LDA nz6_blt_py
        SBC nz6_pic_tmp
        BCC @rts
        STA nz6_blt_py
        LSR A
        LSR A
        STA nz6_blt_celly
@celltop_dst_loop:
        LDA nz6_blt_dst_lo
        SEC
        SBC #<NZ6_GFX_ROW_PIXELS
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        SBC #>NZ6_GFX_ROW_PIXELS
        STA nz6_blt_dst_hi
        DEC nz6_pic_tmp
        BNE @celltop_dst_loop
@flow_icon:
        ; Small Infocom MCGA inline icons are often 21px framed boxes. Place
        ; them inside the six 4px text rows that their height naturally
        ; reserves: one pixel of padding above, two below.
        LDA nz6_pic_flags
        AND #NZ6_PIC_FLAG_FLOW_ICON
        BEQ @after_flow_icon
        LDA nz6_pic_w_hi
        ORA nz6_pic_h_hi
        BNE @after_flow_icon
        LDA #32
        CMP nz6_pic_w_lo
        BCC @after_flow_icon
        CMP nz6_pic_h_lo
        BCC @after_flow_icon
        SEC
        LDA nz6_blt_py
        SBC #2
        BCC @rts
        STA nz6_blt_py
        LDA nz6_blt_py
        CMP #NZ6_SCREEN_UNIT_ROWS
        BCC @flow_icon_y_ok
        JMP @rts
@flow_icon_y_ok:
        LSR A
        LSR A
        STA nz6_blt_celly
        LDA nz6_blt_dst_lo
        SEC
        SBC #<((NZ6_GFX_ROW_PIXELS * 2) & $FFFF)
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        SBC #>((NZ6_GFX_ROW_PIXELS * 2) & $FFFF)
        STA nz6_blt_dst_hi
@after_flow_icon:
        JMP nz6_pic_draw_current_abs

; Draw the picture already resolved in nz6_pic_* at absolute pixel rect
; nz6_blt_px/nz6_blt_py. Live draws record the operation and may clear text
; cells under opaque art; restore replay only redraws the gfx pixels because
; the saved text planes already contain their final char/colour/attr state.
nz6_pic_draw_current_abs:
        JSR nz6_pic_clip_rect
        BCS :+
        JMP @rts
:
        ; Load the packed bitmap slice into the reusable XRAM bounce buffer.
        LDA #<nz6_pics_name
        STA PAGER_NAMEPTR_L
        LDA #>nz6_pics_name
        STA PAGER_NAMEPTR_H
        LDA #(nz6_pics_name_end - nz6_pics_name)
        STA PAGER_NAMELEN
        LDA nz6_pic_off_l
        STA PAGER_FILEL
        LDA nz6_pic_off_m
        STA PAGER_FILEM
        LDA nz6_pic_off_h
        STA PAGER_FILEH
        LDA nz6_pics_base_l
        STA PAGER_ADDRL
        LDA nz6_pics_base_m
        CLC
        ADC #(ZSTORY_AUX_PICS_BOUNCE_OFF_M - ZSTORY_AUX_PICS_INDEX_OFF_M)
        STA PAGER_ADDRM
        LDA nz6_pics_base_h
        ADC #$00
        STA PAGER_ADDRH
        LDA nz6_pic_len_lo
        STA PAGER_LENL
        LDA nz6_pic_len_hi
        STA PAGER_LENH
        LDA #PAGER_TARGET_XRAM
        STA PAGER_TARGET
        JSR pager_load_file_page
        BEQ :+
@rts:
        RTS
:
        JSR nz6_pic_prefill_flow_cell_rect
        ; Unpack the row-packed 4bpp source into one gfx pixel per VGC
        ; address. BLT_CKEY > 15 disables transparency for opaque pictures.
        LDA #NZ6_SPACE_XRAM
        STA BLT_SRCSPACE
        LDA nz6_pics_base_l
        STA BLT_SRCL
        LDA nz6_pics_base_m
        CLC
        ADC #(ZSTORY_AUX_PICS_BOUNCE_OFF_M - ZSTORY_AUX_PICS_INDEX_OFF_M)
        STA BLT_SRCM
        LDA nz6_pics_base_h
        ADC #$00
        STA BLT_SRCH
        LDA nz6_blt_wbytes
        STA BLT_SRCSTRL
        STZ BLT_SRCSTRH
        LDA #NZ6_SPACE_GFX
        STA BLT_DSTSPACE
        LDA nz6_blt_dst_lo
        STA BLT_DSTL
        LDA nz6_blt_dst_hi
        STA BLT_DSTM
        STZ BLT_DSTH
        LDA #<NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRL
        LDA #>NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRH
        LDA nz6_blt_wclip
        STA BLT_WIDTHL
        LDA nz6_blt_wclip_hi
        STA BLT_WIDTHH
        LDA nz6_blt_hclip
        STA BLT_HEIGHTL
        STZ BLT_HEIGHTH
        LDA nz6_pic_flags
        AND #NZ6_PIC_FLAG_TRANSPARENT
        BEQ @opaque_pic
        LDA nz6_pic_flags
        LSR A
        LSR A
        LSR A
        LSR A
        STA BLT_CKEY
        BRA @start_unpack
@opaque_pic:
        LDA #$FF
        STA BLT_CKEY
@start_unpack:
        JSR blitter_start_gfx4_unpack
        BEQ :+
        RTS
:
        JSR nz6_gfx_record_draw_current
        LDA nz6_gfx_replay_active
        BEQ :+
        JMP @rts
:
        ; MCGA-faithful compositing approximation for the split text/gfx
        ; planes: an opaque picture owns the whole touched cell rectangle, so
        ; blank those cells (bg 0 = display-transparent) and let the gfx plane
        ; show through. Flow pictures also own their reserved text-cell box
        ; even when the source bitmap is keyed transparent. Other transparent
        ; pictures are overlays: expose only space cells so visible text is not
        ; deleted.
@mark_picture_cells:
        JSR nz6_pic_compute_cell_rect
        BCS :+
        JMP @rts
:
        ; borrow the live vtext region for the picture-owned cell rect, then
        ; rebuild the current window's region. Transparent pictures expose only
        ; spaces to the gfx plane. Destructively clearing non-space text creates
        ; holes that later scroll through the transcript.
        LDA nz6_rect_left
        STA VTEXT_LEFT
        LDA nz6_rect_top
        STA VTEXT_TOP
        LDA nz6_rect_w
        STA VTEXT_WIDTH
        LDA nz6_rect_h
        STA VTEXT_HEIGHT
        LDA VTEXT_COLOR
        PHA
        LDA #NZ6_COLOR_DEFAULT          ; bg 0: cells go display-transparent
        STA VTEXT_COLOR
        LDA nz6_pic_flags
        AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)
        BNE @clear_picture_cells
        LDA nz6_pic_flags
        AND #NZ6_PIC_FLAG_TRANSPARENT
        BNE @expose_picture_spaces
@clear_picture_cells:
        JSR vtext_clear_region
        BRA @restore_vtext_color
@expose_picture_spaces:
        JSR vtext_expose_gfx_spaces_region
@restore_vtext_color:
        PLA
        STA VTEXT_COLOR
        JMP nz6_apply_current_window

; Compute the 4x4 text-cell rectangle touched by the current picture rect.
; Partial edge cells belong to the picture because MCGA pixels own that area.
; Out: nz6_rect_left/top/w/h, C=1 if any cells remain after screen clipping.
nz6_pic_compute_cell_rect:
        LDA nz6_blt_cellx
        STA nz6_rect_left
        LDA nz6_blt_celly
        STA nz6_rect_top
        LDA nz6_blt_px_lo               ; cols = ceil((x & 3) + w_px) / 4
        AND #$03
        CLC
        ADC nz6_pic_w_lo
        TAX
        LDA nz6_pic_w_hi
        ADC #0
        STA nz6_pic_tmp
        TXA
        CLC
        ADC #3
        TAX
        LDA nz6_pic_tmp
        ADC #0
        LSR A
        STA nz6_pic_tmp
        TXA
        ROR A
        LSR nz6_pic_tmp
        ROR A
        BEQ @empty
        STA nz6_rect_w
        LDA #NZ6_SCREEN_COLS
        SEC
        SBC nz6_rect_left
        CMP nz6_rect_w
        BCS :+
        STA nz6_rect_w
:
        LDA nz6_rect_w
        BEQ @empty
        LDA nz6_blt_py                  ; rows = ceil((y & 3) + h_px) / 4
        AND #$03
        CLC
        ADC nz6_pic_h_lo
        TAX
        LDA nz6_pic_h_hi
        ADC #0
        STA nz6_pic_tmp
        TXA
        CLC
        ADC #3
        TAX
        LDA nz6_pic_tmp
        ADC #0
        LSR A
        STA nz6_pic_tmp
        TXA
        ROR A
        LSR nz6_pic_tmp
        ROR A
        BEQ @empty
        STA nz6_rect_h
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC nz6_rect_top
        CMP nz6_rect_h
        BCS :+
        STA nz6_rect_h
:
        LDA nz6_rect_h
        BEQ @empty
        SEC
        RTS
@empty:
        CLC
        RTS

; Flow pictures reserve full 4x4 text cells, but their transparent bitmap
; often leaves 1-3 pixels of padding inside the reserved cells. Clear that
; reserved gfx area before the keyed blit so text-transparent padding cannot
; reveal pixels scrolled in from an older inline picture.
nz6_pic_prefill_flow_cell_rect:
        LDA nz6_pic_flags
        AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)
        BNE :+
        RTS
:
        JSR nz6_pic_compute_cell_rect
        BCS :+
        RTS
:
        LDA nz6_gfx_replay_active
        BEQ @live_fill
        LDA nz6_gfx_replay_fill
        BRA @fill_ready
@live_fill:
        LDA nz6_colour                  ; window background colour, 1px/byte
        LSR A
        LSR A
        LSR A
        LSR A
        STA nz6_gfx_replay_fill
@fill_ready:
        STA nz6_gfx_replay_fill
        JMP nz6_fill_gfx_cell_rect

; Fill the gfx pixels under nz6_rect_* cell coordinates with colour A.
nz6_fill_gfx_cell_rect:
        STA nz6_clr_tmp
        LDA nz6_rect_left
        STA VTEXT_LEFT
        LDA nz6_rect_top
        STA VTEXT_TOP
        LDA nz6_rect_w
        STA VTEXT_WIDTH
        LDA nz6_rect_h
        STA VTEXT_HEIGHT
        LDX nz6_clr_tmp
        JMP vtext_fill_gfx_region

; erase_picture N [y x] (EXT:7): fill the picture's (clipped) rect with the
; current window background colour on the gfx layer.
nz6_ext_erase_picture:
        JSR nz6_pics_find
        BCS :+
@rts:
        RTS
:
        JSR nz6_pic_rect_setup
        BCC @rts
        JMP nz6_pic_erase_current_abs

; Erase the picture rect already resolved in nz6_pic_* at absolute pixel rect
; nz6_blt_px/nz6_blt_py. Live erase uses the current background colour and
; records the op; restore replay uses the saved fill colour.
nz6_pic_erase_current_abs:
        JSR nz6_pic_clip_rect
        BCS :+
@rts:
        RTS
:
        STZ BLT_SRCSPACE
        STZ BLT_SRCL
        STZ BLT_SRCM
        STZ BLT_SRCH
        STZ BLT_SRCSTRL
        STZ BLT_SRCSTRH
        LDA #NZ6_SPACE_GFX
        STA BLT_DSTSPACE
        LDA nz6_blt_dst_lo
        STA BLT_DSTL
        LDA nz6_blt_dst_hi
        STA BLT_DSTM
        STZ BLT_DSTH
        LDA #<NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRL
        LDA #>NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRH
        LDA nz6_blt_wclip
        STA BLT_WIDTHL
        LDA nz6_blt_wclip_hi
        STA BLT_WIDTHH
        LDA nz6_blt_hclip
        STA BLT_HEIGHTL
        STZ BLT_HEIGHTH
        LDA #BLT_MODE_FILL
        STA BLT_MODE_REG
        STZ BLT_CKEY
        LDA nz6_gfx_replay_active
        BEQ @live_fill
        LDA nz6_gfx_replay_fill
        BRA @fill_ready
@live_fill:
        LDA nz6_colour                  ; window background colour, 1px/byte
        LSR A
        LSR A
        LSR A
        LSR A
        STA nz6_gfx_replay_fill
@fill_ready:
        STA BLT_FILLVALUE
        LDA #BLT_CMD_START
        STA BLT_CMD_REG
        JSR blitter_wait
        JMP nz6_gfx_record_erase_current

; Resolve the op's window-relative pixel coords + the found picture's dims
; into a gfx rect: C=1 with nz6_blt_* filled in, C=0 fully off-screen (or
; behind the left/top edge — games draw on-screen; partial left/top clips
; are not worth the src-offset machinery).
; Operands: y = op1, x = op2 when present (count >= 3), else the current
; window's cursor props. abs 0-based pixel = window origin + rel - 2.
nz6_pic_rect_setup:
        BRA @begin
@off:
        CLC
        RTS
@begin:
        LDA nz6_win_current
        STA nz6_tmp_win
        LDA zvm_operand_count
        CMP #3
        BCS @explicit
        LDX #4                          ; y := cursor y prop
        JSR nz6_read_prop_unit
        JSR @origin_add_y_px
        BCC @off
        LDX #5                          ; x := cursor x prop
        JSR nz6_read_prop_unit
        BRA @have_x
@explicit:
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        JSR @origin_add_y_px
        BCC @off
        LDA zvm_operand_lo+2
        STA nz6_unit_lo
        LDA zvm_operand_hi+2
        STA nz6_unit_hi
@have_x:
        LDX #1                          ; + window origin x (prop 1) - 2
        JSR @origin_add_px
        BCC @off
        LDA nz6_unit_hi
        CMP #>NZ6_SCREEN_UNIT_COLS
        BCC @x_on_screen
        BNE @off
        LDA nz6_unit_lo
        CMP #<NZ6_SCREEN_UNIT_COLS
        BCS @off
@x_on_screen:
        LDA nz6_unit_lo
        STA nz6_blt_px_lo
        LDA nz6_unit_hi
        STA nz6_blt_px_hi
        ; Text invalidation starts at the containing cell.
        LDA nz6_blt_px_lo
        STA nz6_unit_lo
        LDA nz6_blt_px_hi
        STA nz6_unit_hi
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_lo
        STA nz6_blt_cellx
        ; start pixel address = y*320 + x = y*256 + y*64 + x.
        LDA nz6_blt_py
        STA nz6_blt_dst_hi
        STZ nz6_blt_dst_lo
        LDA nz6_blt_py
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        STA nz6_pic_tmp                 ; low byte of y*64
        LDA nz6_blt_py
        LSR A
        LSR A                           ; high byte of y*64
        CLC
        ADC nz6_blt_dst_hi
        STA nz6_blt_dst_hi
        CLC
        LDA nz6_blt_dst_lo
        ADC nz6_pic_tmp
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        ADC #0
        STA nz6_blt_dst_hi
        CLC
        LDA nz6_blt_dst_lo
        ADC nz6_blt_px_lo
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        ADC nz6_blt_px_hi
        STA nz6_blt_dst_hi
@source_row_bytes:
        ; source bytes per row = (w_px + 1) / 2 (max 160)
        LDA nz6_pic_w_lo
        CLC
        ADC #1
        STA nz6_blt_wbytes
        LDA nz6_pic_w_hi
        ADC #0
        LSR A
        ROR nz6_blt_wbytes
        SEC
        RTS

; nz6_unit (16-bit rel y) + window origin prop0 - 2 -> nz6_blt_py/celly.
; C=0 when the result is negative or below the screen.
@origin_add_y_px:
        LDX #0
        JSR @origin_add_px
        BCC @bad_y
        LDA nz6_unit_hi
        BNE @bad_y
        LDA nz6_unit_lo
        CMP #NZ6_SCREEN_UNIT_ROWS
        BCS @bad_y
        STA nz6_blt_py
        LSR A
        LSR A
        STA nz6_blt_celly
        SEC
        RTS
@bad_y:
        CLC
        RTS

; abs pixel = window origin prop X (1-based unit) + nz6_unit (1-based rel)
; - 2. Returns nz6_unit_lo/hi with C=1, or C=0 when negative/huge.
@origin_add_px:
        LDA nz6_unit_lo
        PHA
        LDA nz6_unit_hi
        PHA
        JSR nz6_read_prop_unit          ; clobbers nz6_unit with the prop
        PLA
        TAX                             ; rel hi
        PLA                             ; rel lo
        CLC
        ADC nz6_unit_lo
        STA nz6_unit_lo
        TXA
        ADC nz6_unit_hi
        STA nz6_unit_hi
        BCS @oa_bad
        SEC
        LDA nz6_unit_lo
        SBC #2
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC #0
        STA nz6_unit_hi
        BCC @oa_bad
        SEC
        RTS
@oa_bad:
        CLC
        RTS

; Resolve an already-final absolute pixel rect into cell/destination scratch.
; Used only by restore replay; live draw/erase still goes through the
; window-relative V6 coordinate path above.
nz6_pic_abs_rect_setup:
        LDA nz6_blt_py
        CMP #NZ6_SCREEN_UNIT_ROWS
        BCC :+
        JMP @off
:
        LDA nz6_blt_px_hi
        CMP #>NZ6_SCREEN_UNIT_COLS
        BCC @x_on_screen
        BEQ :+
        JMP @off
:
        LDA nz6_blt_px_lo
        CMP #<NZ6_SCREEN_UNIT_COLS
        BCC @x_on_screen
        JMP @off
@x_on_screen:
        LDA nz6_blt_py
        LSR A
        LSR A
        STA nz6_blt_celly
        LDA nz6_blt_px_lo
        STA nz6_unit_lo
        LDA nz6_blt_px_hi
        STA nz6_unit_hi
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LSR nz6_unit_hi
        ROR nz6_unit_lo
        LDA nz6_unit_lo
        STA nz6_blt_cellx
        ; start pixel address = y*320 + x = y*256 + y*64 + x.
        LDA nz6_blt_py
        STA nz6_blt_dst_hi
        STZ nz6_blt_dst_lo
        LDA nz6_blt_py
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        ASL A
        STA nz6_pic_tmp
        LDA nz6_blt_py
        LSR A
        LSR A
        CLC
        ADC nz6_blt_dst_hi
        STA nz6_blt_dst_hi
        CLC
        LDA nz6_blt_dst_lo
        ADC nz6_pic_tmp
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        ADC #0
        STA nz6_blt_dst_hi
        CLC
        LDA nz6_blt_dst_lo
        ADC nz6_blt_px_lo
        STA nz6_blt_dst_lo
        LDA nz6_blt_dst_hi
        ADC nz6_blt_px_hi
        STA nz6_blt_dst_hi
        ; source bytes per row = (w_px + 1) / 2 (max 160)
        LDA nz6_pic_w_lo
        CLC
        ADC #1
        STA nz6_blt_wbytes
        LDA nz6_pic_w_hi
        ADC #0
        LSR A
        ROR nz6_blt_wbytes
        SEC
        RTS
@off:
        CLC
        RTS

; Clip the found picture's PIXEL rect against the plane (erase_picture's
; fill needs explicit bounds; draw clips host-side). C=0 = nothing visible.
nz6_pic_clip_rect:
        ; width: min(w_px, 320 - px), 16-bit
        SEC
        LDA #<NZ6_GFX_ROW_PIXELS
        SBC nz6_blt_px_lo
        TAX                             ; avail lo
        LDA #>NZ6_GFX_ROW_PIXELS
        SBC nz6_blt_px_hi               ; avail hi (px < 320, never borrows)
        TAY
        ; compare avail (Y:X) with w (hi:lo): keep the smaller
        CPY nz6_pic_w_hi
        BCC @use_avail
        BNE @use_w
        CPX nz6_pic_w_lo
        BCC @use_avail
@use_w:
        LDA nz6_pic_w_lo
        STA nz6_blt_wclip
        LDA nz6_pic_w_hi
        STA nz6_blt_wclip_hi
        BRA @width_done
@use_avail:
        STX nz6_blt_wclip
        STY nz6_blt_wclip_hi
@width_done:
        LDA nz6_blt_wclip
        ORA nz6_blt_wclip_hi
        BEQ @empty
        ; height: min(h_px, 200 - py), 8-bit (both <= 200)
        LDA #NZ6_GFX_ROWS
        SEC
        SBC nz6_blt_py
        STA nz6_pic_tmp                 ; rows available
        LDA nz6_pic_h_hi
        BNE @h_clamp
        LDA nz6_pic_h_lo
        CMP nz6_pic_tmp
        BCC @h_have
        BEQ @h_have
@h_clamp:
        LDA nz6_pic_tmp
@h_have:
        STA nz6_blt_hclip
        LDA nz6_blt_hclip              ; STA does not refresh Z; exact-fit
                                       ; clips arrive here with Z still set.
        BEQ @empty
        SEC
        RTS
@empty:
        CLC
        RTS

; --- Retained graphics display state -----------------------------------------
;
; Save/restore stores the V6 BSS block. Keep a compact operation list inside
; that block so restore can rebuild the gfx plane from native PICS.PAK assets
; instead of serializing the raw 320x200 framebuffer.

nz6_gfx_note_clear:
        LDA nz6_gfx_replay_active
        BNE @rts
        LDA nz6_clr_tmp
        STA nz6_gfx_base_color
        STZ nz6_gfx_list_count
@rts:
        RTS

nz6_gfx_record_alloc:
        LDA nz6_gfx_replay_active
        BNE @full
        LDX nz6_gfx_list_count
        CPX #NZ6_GFX_LIST_MAX
        BCS @full
        INC nz6_gfx_list_count
        SEC
        RTS
@full:
        CLC
        RTS

nz6_gfx_record_draw_current:
        JSR nz6_gfx_record_alloc
        BCC @rts
        LDA #NZ6_GFX_OP_DRAW
        STA nz6_gfx_list_op,X
        LDA zvm_operand_lo
        STA nz6_gfx_list_pic_lo,X
        LDA zvm_operand_hi
        STA nz6_gfx_list_pic_hi,X
        LDA nz6_blt_px_lo
        STA nz6_gfx_list_x_lo,X
        LDA nz6_blt_px_hi
        STA nz6_gfx_list_x_hi,X
        LDA nz6_blt_py
        STA nz6_gfx_list_y,X
        LDA nz6_pic_flags
        AND #(NZ6_PIC_FLAG_FLOW_ICON | NZ6_PIC_FLAG_FLOW_CELLTOP)
        BEQ @no_fill
        LDA nz6_gfx_replay_fill
        BRA @store_fill
@no_fill:
        LDA #$00
@store_fill:
        STA nz6_gfx_list_fill,X
@rts:
        RTS

nz6_gfx_record_erase_current:
        JSR nz6_gfx_record_alloc
        BCC @rts
        LDA #NZ6_GFX_OP_ERASE
        STA nz6_gfx_list_op,X
        LDA zvm_operand_lo
        STA nz6_gfx_list_pic_lo,X
        LDA zvm_operand_hi
        STA nz6_gfx_list_pic_hi,X
        LDA nz6_blt_px_lo
        STA nz6_gfx_list_x_lo,X
        LDA nz6_blt_px_hi
        STA nz6_gfx_list_x_hi,X
        LDA nz6_blt_py
        STA nz6_gfx_list_y,X
        LDA nz6_gfx_replay_fill
        STA nz6_gfx_list_fill,X
@rts:
        RTS

nz6_gfx_clamp_list:
        LDA nz6_gfx_list_count
        CMP #(NZ6_GFX_LIST_MAX + 1)
        BCC @rts
        LDA #NZ6_GFX_LIST_MAX
        STA nz6_gfx_list_count
@rts:
        RTS

nz6_op_restore_display:
        ; Saves keep the logical retained picture list, not the native picture
        ; index/cache. Rebuild PICS.PAK state before replaying so restore works
        ; after reboot and after save/load UI scratch activity.
        JSR nz6_pics_preload
        JSR nz6_gfx_clamp_list
        LDA #$01
        STA nz6_gfx_replay_active
        LDA nz6_gfx_base_color
        STA VGC_BORDER
        LDA nz6_gfx_base_color
        JSR nz6_gfx_clear
        STZ nz6_gfx_replay_index
@loop:
        LDX nz6_gfx_replay_index
        CPX nz6_gfx_list_count
        BCC :+
        BRA @done
:
        LDA nz6_gfx_list_op,X
        CMP #NZ6_GFX_OP_DRAW
        BEQ @draw
        CMP #NZ6_GFX_OP_ERASE
        BEQ @erase
        BRA @next
@draw:
        JSR nz6_gfx_replay_load_entry
        JSR nz6_gfx_replay_draw
        BRA @next
@erase:
        JSR nz6_gfx_replay_load_entry
        JSR nz6_gfx_replay_erase
@next:
        INC nz6_gfx_replay_index
        BRA @loop
@done:
        STZ nz6_gfx_replay_active
        JMP zvm_clear_whole_screen

nz6_gfx_replay_load_entry:
        LDX nz6_gfx_replay_index
        LDA nz6_gfx_list_pic_lo,X
        STA zvm_operand_lo
        LDA nz6_gfx_list_pic_hi,X
        STA zvm_operand_hi
        LDA nz6_gfx_list_x_lo,X
        STA nz6_blt_px_lo
        LDA nz6_gfx_list_x_hi,X
        STA nz6_blt_px_hi
        LDA nz6_gfx_list_y,X
        STA nz6_blt_py
        LDA nz6_gfx_list_fill,X
        STA nz6_gfx_replay_fill
        RTS

nz6_gfx_replay_draw:
        JSR nz6_pics_find
        BCS :+
@rts:
        RTS
:
        LDA nz6_pic_len_lo
        ORA nz6_pic_len_hi
        BEQ @rts
        JSR nz6_pic_abs_rect_setup
        BCC @rts
        JMP nz6_pic_draw_current_abs

nz6_gfx_replay_erase:
        JSR nz6_pics_find
        BCS :+
@rts:
        RTS
:
        JSR nz6_pic_abs_rect_setup
        BCC @rts
        JMP nz6_pic_erase_current_abs

; Fill the whole 320x200 gfx plane with colour A. The V6 erase paths pass
; the window background — the gfx plane IS the MCGA framebuffer, so the
; parchment field lives here and partial picture-edge cells reveal it.
; Reset passes 0 (display-transparent).
nz6_gfx_fill_bg:
        JSR nz6_sync_border_to_bg
        LDA nz6_colour                  ; window background colour
        LSR A
        LSR A
        LSR A
        LSR A
nz6_gfx_clear:
        STA nz6_clr_tmp
        JSR nz6_gfx_note_clear
        STZ BLT_SRCSPACE
        STZ BLT_SRCL
        STZ BLT_SRCM
        STZ BLT_SRCH
        STZ BLT_SRCSTRL
        STZ BLT_SRCSTRH
        LDA #NZ6_SPACE_GFX
        STA BLT_DSTSPACE
        STZ BLT_DSTL
        STZ BLT_DSTM
        STZ BLT_DSTH
        LDA #<NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRL
        STA BLT_WIDTHL                  ; 320 px wide, one byte per pixel
        LDA #>NZ6_GFX_ROW_PIXELS
        STA BLT_DSTSTRH
        STA BLT_WIDTHH
        LDA #NZ6_GFX_ROWS
        STA BLT_HEIGHTL
        STZ BLT_HEIGHTH
        LDA #BLT_MODE_FILL
        STA BLT_MODE_REG
        STZ BLT_CKEY
        LDA nz6_clr_tmp                 ; the fill colour passed in A
        STA BLT_FILLVALUE
        LDA #BLT_CMD_START
        STA BLT_CMD_REG
        JMP blitter_wait

; Read the index byte at the 24-bit cursor -> A; cursor += 1.
nz6_pics_next_byte:
        LDA nz6_pics_cur_l
        STA XRAM_ADDRL
        LDA nz6_pics_cur_m
        STA XRAM_ADDRM
        LDA nz6_pics_cur_h
        STA XRAM_ADDRH
        JSR xram_read8
        INC nz6_pics_cur_l
        BNE :+
        INC nz6_pics_cur_m
        BNE :+
        INC nz6_pics_cur_h
:
        LDA XRAM_DATA
        RTS

; read_mouse table (EXT:22). No mouse hardware: y, x, buttons and menu word
; all read back 0. No store, no branch.
nz6_ext_read_mouse:
        LDA zvm_operand_lo
        STA zstory_addr_l
        LDA zvm_operand_hi
        STA zstory_addr_m
        STZ zstory_addr_h
        LDX #4
@loop:
        PHX
        JSR nz6_write_zero_word
        PLX
        DEX
        BNE @loop
        RTS

; Palette helpers are linked from runtime/asm/build/nova.lib.

; --- Dispatch tables ---------------------------------------------------------

nz6_var_table:                  ; ids $00-$0B
        .word nz6_op_reset      ; $00 reset (ROM invokes per game start)
        .word nz6_op_split      ; $01 split_window
        .word nz6_op_set_window ; $02 set_window
        .word nz6_op_erase      ; $03 erase_window
        .word nz6_op_set_cursor ; $04 set_cursor
        .word nz6_op_get_cursor ; $05 get_cursor (table operand, no store)
        .word nz6_op_set_colour ; $06 set_colour
        .word nz6_var_pull      ; $07 pull (V6 store form, optional user stack)
        .word nz6_apply_current_window ; $08 select (zvm_select_active_window V6 path)
        .word nz6_op_text_style ; $09 set_text_style (colour-relative styles)
        .word nz6_op_cr_newline ; $0A newline hook (CR-interrupt countdown)
        .word nz6_op_pre_newline_scroll ; $0B pre-scroll gfx before newline
        .word nz6_op_restore_display ; $0C replay retained V6 graphics
.assert (* - nz6_var_table) / 2 = NZ6_OP_RESTORE_DISPLAY + 1, error, "nz6_var_table must cover ids 0..NZ6_OP_RESTORE_DISPLAY"

nz6_ext_table:                  ; ext opnums 0-29; only 5-8 and 16-29 arrive
        .word nz6_bug           ;  0 save (ROM handles)
        .word nz6_bug           ;  1 restore (ROM handles)
        .word nz6_bug           ;  2 log_shift (ROM handles)
        .word nz6_bug           ;  3 art_shift (ROM handles)
        .word nz6_bug           ;  4 set_font (ROM handles)
        .word nz6_ext_draw_picture ;  5 draw_picture (XPAGE -> bounce -> blit)
        .word nz6_ext_picture_data ;  6 picture_data (real index answers)
        .word nz6_ext_erase_picture ;  7 erase_picture (bg rect fill)
        .word nz6_ext_set_margins ;  8 set_margins
        .word nz6_bug           ;  9 save_undo (ROM handles)
        .word nz6_bug           ; 10 restore_undo (ROM handles)
        .word nz6_bug           ; 11 print_unicode (ROM: unsupported)
        .word nz6_bug           ; 12 check_unicode (ROM: unsupported)
        .word nz6_bug           ; 13 set_true_colour (ROM: unsupported)
        .word nz6_bug           ; 14 (unassigned)
        .word nz6_bug           ; 15 (unassigned)
        .word nz6_ext_move_window   ; 16 move_window
        .word nz6_ext_window_size   ; 17 window_size
        .word nz6_ext_window_style  ; 18 window_style
        .word nz6_ext_get_wind_prop ; 19 get_wind_prop (store)
        .word nz6_ext_scroll_window ; 20 scroll_window (cells; up/forward only)
        .word nz6_ext_pop_stack ; 21 pop_stack (discards items, no store/branch)
        .word nz6_ext_read_mouse ; 22 read_mouse (no mouse: four zero words)
        .word nz6_stub          ; 23 mouse_window (meaningless without a mouse)
        .word nz6_ext_push_stack ; 24 push_stack (branches true on success)
        .word nz6_ext_put_wind_prop ; 25 put_wind_prop
        .word nz6_stub          ; 26 print_form (formatted-table print: M2)
        .word nz6_branch_false  ; 27 make_menu (no menu capability: branch always false)
        .word nz6_stub          ; 28 picture_table (cache hint only; no pictures to cache)
        .word nz6_store_zero    ; 29 buffer_screen (store; always reports unbuffered mode 0)
.assert (* - nz6_ext_table) / 2 = NZ6_EXT_COUNT, error, "nz6_ext_table must have NZ6_EXT_COUNT entries"

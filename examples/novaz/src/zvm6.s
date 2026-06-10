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
;   - Coordinates/cursors are 1-based UNITS, and 1 unit = 1 TEXT CELL: the
;     header (zstory.s) advertises an 80x50-unit screen with a 1x1 font, so
;     games compute and send cell coordinates (Zork Zero capture finding 1).
;     cell = unit-1, unit = cell+1; M3 converts cells to gfx pixels (x4,
;     exact) at the picture-blit boundary only.
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

.setcpu "65c02"

.include "zvm6.inc"
.include "runtime_abi.inc"      ; ROM ABI addresses; must follow zvm6.inc

; Screen geometry (cells). Mirrors VTEXT_SCREEN_COLS/ROWS (runtime/asm/
; vtext.inc); single change point if the text grid ever reparameterizes.
NZ6_SCREEN_ROWS = 50
NZ6_SCREEN_COLS = 80

; Keep in sync with ZVM_COLOR_NORMAL in src/zvm.s (equates do not appear in
; runtime.sym, so it cannot ride the generated ABI include).
NZ6_COLOR_NORMAL = $0C

.segment "BSS"

; 8 windows x 16 properties x 2 bytes, little-endian words.
nz6_win_props:   .res 8*16*2
nz6_win_current: .res 1         ; current V6 window, 0-7
nz6_tmp_win:     .res 1         ; window targeted by the op in flight
nz6_tmp_off:     .res 1         ; scratch for table offset math
nz6_tmp_lines:   .res 1         ; split_window: clamped units
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

.segment "CODE"

        .byte NZ6_MAGIC0, NZ6_MAGIC1   ; $2000: magic word

; A = dispatch id (see zvm6.inc):
;   $00-$06           VAR screen ops → nz6_var_table
;   $20 + ext opnum   EXT ops → nz6_ext_table (opnums 0-29)
; Dispatch via 65C02 JMP (abs,X): each handler RTSes straight back to the
; interpreter step loop, exactly like a ROM table handler would.
nz6_entry:                              ; $2002: dispatch entry, A = id
        JMP nz6_dispatch

.assert nz6_entry = NZ6_ENTRY, error, "nz6_entry must sit at NZ6_ENTRY ($2002)"

nz6_dispatch:
        CMP #NZ6_EXT_BASE
        BCS @ext
        CMP #NZ6_OP_SELECT + 1
        BCS nz6_bug             ; reserved VAR ids $09-$1F are never routed
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

; nz6_unit := 1 (or 80): origin and full-width constants; units are cells.
nz6_unit_one:
        LDA #1
        STA nz6_unit_lo
        STZ nz6_unit_hi
        RTS

nz6_unit_80:
        LDA #NZ6_SCREEN_COLS
        STA nz6_unit_lo
        STZ nz6_unit_hi
        RTS

; Window number from operand index X -> A (0-7), C=1 valid / C=0 invalid.
; Preserves X.
nz6_window_from_operand:
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

; --- unit <-> cell conversions (units ARE cells; only the 1-based offset
; --- and the window origin move) ----------------------------------------------

; cell = unit - 1 for the 16-bit unit in nz6_unit_lo/hi -> A.
; unit 0 (never legal, units are 1-based) maps to cell 0; results past 255
; clamp to $FF (the ROM's own cursor clamps bound further). Clobbers nz6_unit.
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
        BEQ :+
        LDA #$FF
        RTS
:
        LDA nz6_unit_lo
        RTS
@zero:
        LDA #0
        RTS

; unit = cell + 1 for the cell in A -> nz6_unit_lo/hi.
nz6_cell_to_unit:
        STZ nz6_unit_hi
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

; --- the current window IS the live vtext region -------------------------------

; Rebuild the live vtext region from the CURRENT window's props and
; reposition the visible cursor. This replaces the ROM's lower/upper split
; logic for V6: zvm_select_active_window routes here (NZ6_OP_SELECT), and
; every segment op that changes the current window's geometry or cursor
; tail-calls it. Mapping (units are cells):
;   VTEXT_TOP    = prop0 - 1                        clamped onto the screen
;   VTEXT_LEFT   = prop1 - 1 + left margin (prop6)  clamped onto the screen
;   VTEXT_HEIGHT = prop2                  clamped to the rows below TOP
;   VTEXT_WIDTH  = prop3 - both margins   clamped to the cols right of LEFT
;   VTEXT_FLAGS  = attr (prop14) & 3 — Z attribute bit 0 (wrap) and bit 1
;                  (scroll) line up exactly with VTEXT_FLAG_WRAP/SCROLL.
;                  Window 0 defaults to both, windows 1-7 to neither.
;   VTEXT_CURX/Y = props 5/4 - 1, clamped inside WIDTH/HEIGHT.
; Cursor props 4/5 are 1-based and relative to the margin-shrunk writable
; area. A zero-sized window leaves an invalid region: vtext drops output
; until the game sizes it (games size windows before drawing into them).
nz6_apply_current_window:
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
nz6_build_region_tmp_win:
        ; TOP
        LDX #0
        JSR nz6_prop_cell
        CMP #NZ6_SCREEN_ROWS
        BCC :+
        LDA #NZ6_SCREEN_ROWS-1
:
        STA VTEXT_TOP
        ; margins
        LDX #6
        JSR nz6_prop_byte
        STA nz6_marg_l
        LDX #7
        JSR nz6_prop_byte
        STA nz6_marg_r
        ; LEFT = origin cell + left margin
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
        JSR nz6_prop_byte
        STA VTEXT_HEIGHT
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC VTEXT_TOP
        CMP VTEXT_HEIGHT
        BCS :+
        STA VTEXT_HEIGHT
:
        ; WIDTH = prop3 - margins (floor 0), clamped to the cols right of LEFT
        LDX #3
        JSR nz6_prop_byte
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
; path. Window 0: full screen (1,1 / 50x80), wrap+scroll+buffer, cursor
; 1,1. Windows 1-7: origin 1,1, size 0, no attributes, cursor 1,1. All
; windows: font 1, font size 1x1 (units are cells). Current window 0.
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
        STA nz6_win_props+26,Y         ; prop 13: font size = $0101 (1x1 units)
        STA nz6_win_props+27,Y
        DEX
        BPL @win
        LDA #NZ6_SCREEN_ROWS
        STA nz6_win_props+4            ; window 0 prop 2: y-size = 50
        LDA #NZ6_SCREEN_COLS
        STA nz6_win_props+6            ; window 0 prop 3: x-size = 80
        LDA #%00001011
        STA nz6_win_props+28           ; window 0 prop 14: wrap+scroll+buffer
        STZ nz6_win_current
        STZ zvm_window_current         ; word-buffer gate: window 0 live
        RTS

; Dispatch id NZ6_OP_RESET: the ROM invokes this from zvm_run_until_read's
; V6 branch on every game (re)start.
nz6_op_reset:
        JSR nz6_reset_windows
        JMP nz6_apply_current_window

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
        ; window 1: origin 1,1, y-size = lines, x-size = 80
        LDA #1
        STA nz6_tmp_win
        LDA nz6_tmp_lines
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
        ; window 0: origin 1,lines+1, y-size = 50-lines, x-size = 80
        STZ nz6_tmp_win
        LDA nz6_tmp_lines
        INC A
        STA nz6_unit_lo
        STZ nz6_unit_hi
        LDX #0
        JSR nz6_write_prop_unit
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC nz6_tmp_lines
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
        JMP zvm_clear_whole_screen      ; (tail-calls the V6 select -> apply)
:
        CMP #$FE
        BNE @rts
        JSR nz6_sync_live_cursor        ; -2 must leave the cursor alone
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
        ; cell rect from props 0-3; skip the fill if it lies off-grid or is
        ; empty
        LDX #1
        JSR nz6_prop_cell
        CMP #NZ6_SCREEN_COLS
        BCC :+
        JMP @apply
:
        STA nz6_rect_left
        LDX #0
        JSR nz6_prop_cell
        CMP #NZ6_SCREEN_ROWS
        BCC :+
        JMP @apply
:
        STA nz6_rect_top
        LDX #3
        JSR nz6_prop_byte
        STA nz6_rect_w
        LDA #NZ6_SCREEN_COLS
        SEC
        SBC nz6_rect_left
        CMP nz6_rect_w
        BCS :+
        STA nz6_rect_w
:
        LDA nz6_rect_w
        BEQ @apply
        LDX #2
        JSR nz6_prop_byte
        STA nz6_rect_h
        LDA #NZ6_SCREEN_ROWS
        SEC
        SBC nz6_rect_top
        CMP nz6_rect_h
        BCS :+
        STA nz6_rect_h
:
        LDA nz6_rect_h
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
        LDA #NZ6_COLOR_NORMAL
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

; set_cursor y x [window] (VAR:15). Units (= cells), relative to the window
; origin. Row/col 0 clamps to 1: Zork Zero sends set_cursor 0,1 in its
; pre-read banner block (units are 1-based, 0 is off-grid). Only the table
; changes for a non-current window; the live cursor moves only when the
; target IS the current window.
nz6_op_set_cursor:
        LDA zvm_operand_hi
        ORA zvm_operand_hi+1
        BMI @rts                        ; negative y/x: V6 cursor on/off — out of M1 scope
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
        LDA zvm_operand_lo
        STA nz6_unit_lo
        LDA zvm_operand_hi
        STA nz6_unit_hi
        ORA zvm_operand_lo
        BNE :+
        INC nz6_unit_lo                 ; row 0 -> 1
:
        LDX #4
        JSR nz6_write_prop_unit
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        ORA zvm_operand_lo+1
        BNE :+
        INC nz6_unit_lo                 ; col 0 -> 1
:
        LDX #5
        JSR nz6_write_prop_unit
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @rts
        JMP nz6_apply_current_window
@rts:
        RTS

; get_cursor array (VAR:16). Writes the CURRENT window's cursor as two
; words (y then x, units = cells, window-relative, exactly as stored in
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

; set_colour fg bg [window] (2OP:27). M1 records the colour data in prop 11
; ((bg<<8)|fg) without touching the live VGC palette. An absent or
; out-of-range window operand (including the -3 "current window" idiom)
; targets the current window.
nz6_op_set_colour:
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
        JMP nz6_write_prop_unit

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
        JMP nz6_geom_epilogue
@rts:
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

; scroll_window window pixels (EXT:20). Amounts are CELLS (units = cells,
; M2): a positive amount scrolls the window's text up/forward (Z-spec 1.1),
; and the vacated bottom rows come back blank in the live text colour (M2's
; single-colour model — per-window colour rendering, prop 11, is out of M2
; scope). Works on ANY window: the target's margin-shrunk writable rect —
; the same rect implicit scrolling uses — is borrowed into the live vtext
; state, scrolled, and the current window's region rebuilt afterwards. The
; target's cursor props do NOT move (Zork Zero always follows its page-reset
; scroll with set_cursor). Amounts >= the window height degenerate to a
; whole-rect clear. Negative amounts (scroll down/backward): vtext has no
; descending row blit and Zork Zero never sends one (capture: its single
; call is positive), so fail loudly through the ROM's unsupported-opcode
; path rather than silently mis-rendering.
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
        JSR nz6_build_region_tmp_win    ; target rect -> live vtext state
        LDA zvm_operand_hi+1
        BNE @clear                      ; > 255 cells: blank the whole rect
        LDA zvm_operand_lo+1
        CMP VTEXT_HEIGHT
        BCS @clear                      ; >= height: ditto (height 0 too)
        TAX
@loop:
        PHX
        JSR vtext_scroll_up
        PLX
        DEX
        BNE @loop
@restore:
        JMP nz6_apply_current_window
@clear:
        JSR vtext_clear_region
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

; picture_data N table ?(label) (EXT:6). M1 ships no picture file: with N=0
; ("how many pictures?") the spec requires word 0 := picture count and
; word 1 := release number — both 0 here. With N>0 (a specific picture)
; nothing is written. Either way no picture data is available, so the
; branch — "is there data?" — is always FALSE.
nz6_ext_picture_data:
        LDA zvm_operand_lo
        ORA zvm_operand_hi
        BNE @branch
        JSR nz6_ustack_addr             ; table = operand 1, same idiom
        JSR nz6_write_zero_word         ; word 0: picture count = 0
        JSR nz6_write_zero_word         ; word 1: release number = 0
@branch:
        JMP zvm_branch_false

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

; --- Dispatch tables ---------------------------------------------------------

nz6_var_table:                  ; ids $00-$07
        .word nz6_op_reset      ; $00 reset (ROM invokes per game start)
        .word nz6_op_split      ; $01 split_window
        .word nz6_op_set_window ; $02 set_window
        .word nz6_op_erase      ; $03 erase_window
        .word nz6_op_set_cursor ; $04 set_cursor
        .word nz6_op_get_cursor ; $05 get_cursor (table operand, no store)
        .word nz6_op_set_colour ; $06 set_colour
        .word nz6_var_pull      ; $07 pull (V6 store form, optional user stack)
        .word nz6_apply_current_window ; $08 select (zvm_select_active_window V6 path)
.assert (* - nz6_var_table) / 2 = NZ6_OP_SELECT + 1, error, "nz6_var_table must cover ids 0..NZ6_OP_SELECT"

nz6_ext_table:                  ; ext opnums 0-29; only 5-8 and 16-29 arrive
        .word nz6_bug           ;  0 save (ROM handles)
        .word nz6_bug           ;  1 restore (ROM handles)
        .word nz6_bug           ;  2 log_shift (ROM handles)
        .word nz6_bug           ;  3 art_shift (ROM handles)
        .word nz6_bug           ;  4 set_font (ROM handles)
        .word nz6_stub          ;  5 draw_picture (no pictures: drawing nothing is correct)
        .word nz6_ext_picture_data ;  6 picture_data (N=0 writes count/release; branch false)
        .word nz6_stub          ;  7 erase_picture (no pictures: nothing was ever drawn)
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

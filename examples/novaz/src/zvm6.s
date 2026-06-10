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
; Window model (M1, Z-Machine Standard 1.1 section 8.8):
;   - 8 windows, 16 word-sized properties each, stored little-endian in
;     nz6_win_props (offset = window*32 + prop*2).
;   - Coordinates/cursors are 1-based UNITS. 1 unit = 1 gfx pixel; the font
;     is 4x4 units, so the 320x200-unit screen is the 80x50 text cell grid:
;     cell = (unit-1)>>2, unit = cell*4+1.
;   - Mapping onto the ROM's two-window text path: window 0 drives the
;     classic LOWER window (region-relative cell cursor zvm_lower_x/y,
;     region top = zvm_split_lines); windows 1-7 drive the classic UPPER
;     window (absolute cell cursor zvm_upper_x/y, no scroll). The table is
;     the source of truth for cursors of non-live windows; the live ROM
;     cursor is synced back into the table at window switches and reads.
;   - Out of M1 scope (stored but not acted on): margins, newline
;     interrupts, transcript bit, line counts, colour data, font props.

.setcpu "65c02"

.include "zvm6.inc"
.include "runtime_abi.inc"      ; ROM ABI addresses; must follow zvm6.inc

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
        CMP #NZ6_OP_PULL + 1
        BCS nz6_bug             ; reserved VAR ids $08-$1F are never routed
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

; nz6_unit := 1 / 320 (origin and full-width constants).
nz6_unit_one:
        LDA #1
        STA nz6_unit_lo
        STZ nz6_unit_hi
        RTS

nz6_unit_320:
        LDA #<320
        STA nz6_unit_lo
        LDA #>320
        STA nz6_unit_hi
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

; --- unit <-> cell conversions ----------------------------------------------

; cell = (unit-1)>>2 for the 16-bit unit in nz6_unit_lo/hi -> A.
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
        CLC
        ADC #1
        STA nz6_unit_lo
        BCC :+
        INC nz6_unit_hi
:
        RTS

; span cells = (size_units + 3) >> 2 (round up) for nz6_unit -> A, clamped
; to $FF. Clobbers nz6_unit.
nz6_units_to_span:
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

; A = absolute cell, X = origin prop (0 = y, 1 = x) of window nz6_tmp_win.
; -> nz6_unit = window-relative 1-based unit: cell*4+1 - origin + 1, min 1.
nz6_abs_cell_to_rel_unit:
        PHX
        JSR nz6_cell_to_unit            ; nz6_unit = abs unit (cell*4+1)
        PLX
        JSR nz6_off_for_prop            ; Y -> origin prop of nz6_tmp_win
        SEC
        LDA nz6_unit_lo
        SBC nz6_win_props,Y
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC nz6_win_props+1,Y
        STA nz6_unit_hi
        BCC @floor                      ; abs < origin: clamp to unit 1
        INC nz6_unit_lo
        BNE :+
        INC nz6_unit_hi
:
        RTS
@floor:
        LDA #1
        STA nz6_unit_lo
        STZ nz6_unit_hi
        RTS

; X = cursor prop (4 = y, 5 = x) of window nz6_tmp_win; the matching origin
; prop is X-4. -> A = absolute cell: (origin + rel - 2) >> 2.
nz6_rel_prop_to_abs_cell:
        JSR nz6_read_prop_unit          ; nz6_unit = relative unit
        TXA
        SEC
        SBC #4
        TAX
        JSR nz6_off_for_prop            ; Y -> origin prop
        CLC
        LDA nz6_unit_lo
        ADC nz6_win_props,Y
        STA nz6_unit_lo
        LDA nz6_unit_hi
        ADC nz6_win_props+1,Y
        STA nz6_unit_hi
        ; nz6_unit = origin + rel; subtract 1 so nz6_unit_to_cell's own -1
        ; yields (origin + rel - 2) >> 2.
        LDA nz6_unit_lo
        ORA nz6_unit_hi
        BEQ @zero
        SEC
        LDA nz6_unit_lo
        SBC #1
        STA nz6_unit_lo
        LDA nz6_unit_hi
        SBC #0
        STA nz6_unit_hi
        JMP nz6_unit_to_cell
@zero:
        LDA #0
        RTS

; --- live ROM cursor <-> window table sync ------------------------------------

; Copy the live ROM cursor vars into the CURRENT window's cursor props 4/5
; (cells -> 1-based units). Callers run zvm_window_save_cursor first so the
; vars reflect the live VTEXT cursor.
nz6_sync_live_to_table:
        LDA nz6_win_current
        STA nz6_tmp_win
        BNE @upper
        ; window 0 = lower window: zvm_lower_x/y are already relative to the
        ; lower region, whose top tracks window 0's origin.
        LDA zvm_lower_y
        JSR nz6_cell_to_unit
        LDX #4
        JSR nz6_write_prop_unit
        LDA zvm_lower_x
        JSR nz6_cell_to_unit
        LDX #5
        JMP nz6_write_prop_unit
@upper:
        ; windows 1-7 = upper window: zvm_upper_x/y are absolute cells.
        LDA zvm_upper_y
        LDX #0
        JSR nz6_abs_cell_to_rel_unit
        LDX #4
        JSR nz6_write_prop_unit
        LDA zvm_upper_x
        LDX #1
        JSR nz6_abs_cell_to_rel_unit
        LDX #5
        JMP nz6_write_prop_unit

; Load the CURRENT window's cursor props 4/5 into the live ROM cursor vars
; (units -> cells). The caller follows with zvm_select_active_window, which
; clamps and repositions the VTEXT cursor.
nz6_load_table_to_live:
        LDA nz6_win_current
        STA nz6_tmp_win
        BNE @upper
        LDX #4
        JSR nz6_read_prop_unit
        JSR nz6_unit_to_cell
        STA zvm_lower_y
        LDX #5
        JSR nz6_read_prop_unit
        JSR nz6_unit_to_cell
        STA zvm_lower_x
        RTS
@upper:
        LDX #4
        JSR nz6_rel_prop_to_abs_cell
        STA zvm_upper_y
        LDX #5
        JSR nz6_rel_prop_to_abs_cell
        STA zvm_upper_x
        RTS

; --- reset --------------------------------------------------------------------

; Reset the 8-window table to the V6 boot defaults and sync the ROM text
; path. Window 0: full screen (1,1 / 200x320), wrap+scroll+buffer, cursor
; 1,1. Windows 1-7: origin 1,1, size 0, no attributes, cursor 1,1. All
; windows: font 1, font size 4x4. Current window 0.
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
        LDA #$04
        STA nz6_win_props+26,Y         ; prop 13: font size = $0404 (4x4 units)
        STA nz6_win_props+27,Y
        DEX
        BPL @win
        LDA #200
        STA nz6_win_props+4            ; window 0 prop 2: y-size = 200
        LDA #<320
        STA nz6_win_props+6            ; window 0 prop 3: x-size = 320
        LDA #>320
        STA nz6_win_props+7
        LDA #%00001011
        STA nz6_win_props+28           ; window 0 prop 14: wrap+scroll+buffer
        STZ nz6_win_current
        ; ROM text-path sync: unsplit, lower window live, cursors home.
        STZ zvm_window_current
        STZ zvm_split_lines
        STZ zvm_lower_x
        STZ zvm_lower_y
        STZ zvm_upper_x
        STZ zvm_upper_y
        RTS

; Dispatch id NZ6_OP_RESET: the ROM invokes this from zvm_run_until_read's
; V6 branch on every game (re)start.
nz6_op_reset:
        JSR nz6_reset_windows
        JMP zvm_select_active_window

; --- VAR screen ops -----------------------------------------------------------

; split_window lines_units (VAR:10). Window 1 becomes the top 'lines' units
; (full width); window 0 starts below it. Does NOT clear, does NOT move
; cursors beyond the clamping zvm_select_active_window applies.
nz6_op_split:
        JSR nz_screen_flush_word
        JSR zvm_window_save_cursor
        JSR nz6_sync_live_to_table
        ; clamp the requested units to the 200-unit screen height
        LDA zvm_operand_hi
        BEQ :+
        LDA #200
        BRA @have
:
        LDA zvm_operand_lo
        CMP #201
        BCC @have
        LDA #200
@have:
        STA nz6_tmp_lines
        ; window 1: origin 1,1, y-size = lines, x-size = 320
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
        JSR nz6_unit_320
        LDX #3
        JSR nz6_write_prop_unit
        ; window 0: origin 1,lines+1, y-size = 200-lines, x-size = 320
        STZ nz6_tmp_win
        LDA nz6_tmp_lines
        INC A
        STA nz6_unit_lo
        STZ nz6_unit_hi
        LDX #0
        JSR nz6_write_prop_unit
        LDA #200
        SEC
        SBC nz6_tmp_lines
        STA nz6_unit_lo
        STZ nz6_unit_hi
        LDX #2
        JSR nz6_write_prop_unit
        JSR nz6_unit_one
        LDX #1
        JSR nz6_write_prop_unit
        JSR nz6_unit_320
        LDX #3
        JSR nz6_write_prop_unit
        ; ROM bookkeeping: split cells = units>>2, clamped like the classic
        ; path (max 48 keeps at least two lower-window rows)
        LDA nz6_tmp_lines
        LSR A
        LSR A
        CMP #49
        BCC :+
        LDA #48
:
        STA zvm_split_lines
        BNE :+
        STZ zvm_window_current          ; unsplit collapses onto window 0
        STZ nz6_win_current
:
        JMP zvm_select_active_window

; set_window n (VAR:11). Saves the live cursor into the old window's props,
; restores the new window's cursor, and flips the ROM lower/upper mapping.
nz6_op_set_window:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        PHA
        JSR nz_screen_flush_word
        JSR zvm_window_save_cursor
        JSR nz6_sync_live_to_table
        PLA
        STA nz6_win_current
        BEQ @lower
        LDA #1
        STA zvm_window_current
        BRA @load
@lower:
        STZ zvm_window_current
@load:
        JSR nz6_load_table_to_live
        JMP zvm_select_active_window
@rts:
        RTS

; erase_window n (VAR:13). -1: reset windows + clear all + window 0 live.
; -2: clear all, windows untouched. 0-7: clear that window's cell rect to
; the background and home its cursor.
; M1 SIMPLIFICATION: -2 routes through zvm_clear_whole_screen, which homes
; the live cursor — Z-spec says -2 must leave the cursor alone. Acceptable
; until a game visibly cares. TODO(m2): give -2 a clear path that saves and
; restores the live cursor around zvm_clear_whole_screen.
nz6_op_erase:
        JSR nz_screen_flush_word
        LDA zvm_operand_hi
        CMP #$FF
        BNE @positive
        LDA zvm_operand_lo
        CMP #$FF
        BNE :+
        JSR nz6_reset_windows           ; -1
        JMP zvm_clear_whole_screen
:
        CMP #$FE
        BNE @rts
        JMP zvm_clear_whole_screen      ; -2
@rts:
        RTS
@positive:
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
        JSR nz6_read_prop_unit
        JSR nz6_unit_to_cell
        CMP #80
        BCC :+
        JMP @home
:
        STA nz6_rect_left
        LDX #0
        JSR nz6_read_prop_unit
        JSR nz6_unit_to_cell
        CMP #50
        BCC :+
        JMP @home
:
        STA nz6_rect_top
        LDX #3
        JSR nz6_read_prop_unit
        JSR nz6_units_to_span
        STA nz6_rect_w
        LDA #80
        SEC
        SBC nz6_rect_left
        CMP nz6_rect_w
        BCS :+
        STA nz6_rect_w
:
        LDA nz6_rect_w
        BEQ @home
        LDX #2
        JSR nz6_read_prop_unit
        JSR nz6_units_to_span
        STA nz6_rect_h
        LDA #50
        SEC
        SBC nz6_rect_top
        CMP nz6_rect_h
        BCS :+
        STA nz6_rect_h
:
        LDA nz6_rect_h
        BEQ @home
        ; fill the rect with spaces in the background colour, preserving the
        ; caller-visible text colour/attr (zvm_select_active_window restores
        ; the region registers afterwards)
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
@home:
        ; live cursor home only when the erased window is the current one
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @select
        JSR nz6_load_table_to_live
@select:
        JMP zvm_select_active_window

; set_cursor y x [window] (VAR:15). Units, relative to the window origin.
; Only the table changes for a non-current window; the live cursor moves
; only when the target IS the current window.
; NOTE: the live cursor is cell-quantized (4 units per cell), so
; non-cell-aligned units round down on the next nz6_sync_live_to_table.
; M2 must preserve props 4/5 directly if sub-cell precision is required.
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
        LDX #4
        JSR nz6_write_prop_unit
        LDA zvm_operand_lo+1
        STA nz6_unit_lo
        LDA zvm_operand_hi+1
        STA nz6_unit_hi
        LDX #5
        JSR nz6_write_prop_unit
        LDA nz6_tmp_win
        CMP nz6_win_current
        BNE @rts
        JSR nz6_load_table_to_live
        JMP zvm_select_active_window
@rts:
        RTS

; get_cursor array (VAR:16). Writes the CURRENT window's cursor as two
; words (y then x, units, window-relative) into Z-memory.
nz6_op_get_cursor:
        JSR zvm_window_save_cursor
        JSR nz6_sync_live_to_table      ; leaves nz6_tmp_win = current window
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

; set_margins left right [window] (EXT:8). Stored in props 6/7; M1 does not
; act on margins.
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
        JMP nz6_write_prop_unit
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

; move_window window y x (EXT:16): position -> props 0/1.
nz6_ext_move_window:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        LDX #0
        JMP nz6_yx_from_ops12
@rts:
        RTS

; window_size window y x (EXT:17): size -> props 2/3.
nz6_ext_window_size:
        LDX #0
        JSR nz6_window_from_operand
        BCC @rts
        STA nz6_tmp_win
        LDX #2
        JMP nz6_yx_from_ops12
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
; stores 0. The store byte MUST be consumed on every path.
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
@stack_addr:                            ; zstory_addr := operand 1 (table base)
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
.assert (* - nz6_var_table) / 2 = NZ6_OP_PULL + 1, error, "nz6_var_table must cover ids 0..NZ6_OP_PULL"

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
        .word nz6_stub          ; 20 scroll_window (pixel scroll deferred past M1)
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

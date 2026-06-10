; NOVAZ6.BIN — RAM-resident V6 segment (M1 skeleton).
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

.setcpu "65c02"

.include "zvm6.inc"
.include "runtime_abi.inc"      ; ROM ABI addresses; must follow zvm6.inc

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
        CMP #NZ6_OP_SET_COLOUR + 1
        BCS nz6_bug             ; reserved VAR ids $07-$1F are never routed
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

; --- M1 stubs (Tasks 7-9 replace these with real handlers) ------------------

; Ids the ROM never routes here (VAR ids > $06; ext opnums 0-4 and 9-15 stay
; in the ROM's own table). Reaching this is an ROM<->segment ABI contract
; violation — fail loudly through the ROM's unsupported-opcode path rather
; than silently no-opping a routing bug.
nz6_bug:
        JMP zvm_unsupported

; Plain no-op stub: all operands were already decoded by the ROM and the op
; has no store byte and no branch, so ignoring it is stream-safe.
nz6_stub:
        RTS

; Store-op stub (get_wind_prop EXT:19, buffer_screen EXT:29): MUST consume
; the store byte or the instruction stream derails. Mirrors the ROM's
; zvm_false_store contract: value in zvm_value_lo/hi, zvm_fetch returns the
; store variable number in A, zvm_set_var stores it.
nz6_store_zero:
        STZ zvm_value_lo
        STZ zvm_value_hi
        JSR zvm_fetch
        JMP zvm_set_var

; Branch-op stub (picture_data EXT:6, push_stack EXT:24, make_menu EXT:27):
; zvm_branch_false itself runs zvm_branch_decode (consuming the branch
; byte(s)) and then applies condition "false" — do NOT pre-decode here or
; the branch bytes would be consumed twice. push_stack branching false means
; "stack full" until Task 8 implements real user stacks.
nz6_branch_false:
        JMP zvm_branch_false

; --- Dispatch tables ---------------------------------------------------------

nz6_var_table:                  ; ids $00-$06
        .word nz6_stub          ; $00 reset (ROM invokes after segment load)
        .word nz6_stub          ; $01 split_window
        .word nz6_stub          ; $02 set_window
        .word nz6_stub          ; $03 erase_window
        .word nz6_stub          ; $04 set_cursor
        .word nz6_stub          ; $05 get_cursor (table operand, no store)
        .word nz6_stub          ; $06 set_colour

nz6_ext_table:                  ; ext opnums 0-29; only 5-8 and 16-29 arrive
        .word nz6_bug           ;  0 save (ROM handles)
        .word nz6_bug           ;  1 restore (ROM handles)
        .word nz6_bug           ;  2 log_shift (ROM handles)
        .word nz6_bug           ;  3 art_shift (ROM handles)
        .word nz6_bug           ;  4 set_font (ROM handles)
        .word nz6_stub          ;  5 draw_picture
        .word nz6_branch_false  ;  6 picture_data (branch)
        .word nz6_stub          ;  7 erase_picture
        .word nz6_stub          ;  8 set_margins
        .word nz6_bug           ;  9 save_undo (ROM handles)
        .word nz6_bug           ; 10 restore_undo (ROM handles)
        .word nz6_bug           ; 11 print_unicode (ROM: unsupported)
        .word nz6_bug           ; 12 check_unicode (ROM: unsupported)
        .word nz6_bug           ; 13 set_true_colour (ROM: unsupported)
        .word nz6_bug           ; 14 (unassigned)
        .word nz6_bug           ; 15 (unassigned)
        .word nz6_stub          ; 16 move_window
        .word nz6_stub          ; 17 window_size
        .word nz6_stub          ; 18 window_style
        .word nz6_store_zero    ; 19 get_wind_prop (store)
        .word nz6_stub          ; 20 scroll_window
        .word nz6_stub          ; 21 pop_stack (discards, no store)
        .word nz6_stub          ; 22 read_mouse
        .word nz6_stub          ; 23 mouse_window
        .word nz6_branch_false  ; 24 push_stack (branch; false = stack full)
        .word nz6_stub          ; 25 put_wind_prop
        .word nz6_stub          ; 26 print_form
        .word nz6_branch_false  ; 27 make_menu (branch)
        .word nz6_stub          ; 28 picture_table
        .word nz6_store_zero    ; 29 buffer_screen (store)
.assert (* - nz6_ext_table) / 2 = NZ6_EXT_COUNT, error, "nz6_ext_table must have NZ6_EXT_COUNT entries"

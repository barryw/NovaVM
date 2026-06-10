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
; Ids $09-$1F are reserved for future VAR-op routing; EXT ids start at $20
; so the two ranges can never collide.
NZ6_EXT_BASE        = $20    ; ext dispatch id = NZ6_EXT_BASE + ext opnum
NZ6_EXT_COUNT       = 30     ; ext opnums 0-29; nz6_ext_table length asserted

NZ6_BASE            = $2000  ; segment load address (magic word lives here)
NZ6_ENTRY           = $2002  ; ROM calls JMP/JSR here with A = dispatch id
NZ6_SIZE            = $2000  ; fixed image size (novaz6.cfg fills to 8KB)
NZ6_MAGIC0          = 'N'
NZ6_MAGIC1          = '6'

.endif

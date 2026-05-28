; ext_iface.inc — shared interface between NovaLogo base ROM and extension ROM
;
; These equates define the ZP mailbox used to pass arguments and results
; between the base ROM and the extension ROM via the RAM trampoline.
; Both ROMs include this file so addresses are always consistent.
;
; Addresses $80-$91 are outside the linker-managed ZEROPAGE segment
; but inside true zero page RAM, accessible to both ROM banks.

; --- Command and argument count ---
EXT_CMD         = $80           ; command ID (set by base ROM before trampoline call)
EXT_ARGC        = $81           ; argument count

; --- Argument 0 ---
EXT_ARG0_TYPE   = $82           ; type: VAL_NUMBER=$00, VAL_WORD=$01, VAL_LIST=$02
EXT_ARG0_HI     = $83           ; value high byte
EXT_ARG0_LO     = $84           ; value low byte
EXT_ARG0_FRAC   = $85           ; fractional byte (16.8 fixed point)

; --- Argument 1 ---
EXT_ARG1_TYPE   = $86
EXT_ARG1_HI     = $87
EXT_ARG1_LO     = $88
EXT_ARG1_FRAC   = $89

; --- Argument 2 ---
EXT_ARG2_TYPE   = $8A
EXT_ARG2_HI     = $8B
EXT_ARG2_LO     = $8C
EXT_ARG2_FRAC   = $8D

; --- Result ---
EXT_RESULT_TYPE = $8E           ; return value type
EXT_RESULT_HI   = $8F           ; return value high byte
EXT_RESULT_LO   = $90           ; return value low byte
EXT_RESULT_FRAC = $91           ; return value fractional byte

; --- Command IDs ---
EXT_CMD_TEST    = $01           ; test command: returns arg0 + 1

; --- Value types (match eval_type in eval.s) ---
.ifndef VAL_NUMBER
VAL_NUMBER      = $00
.endif
.ifndef VAL_WORD
VAL_WORD        = $01
.endif
.ifndef VAL_LIST
VAL_LIST        = $02
.endif

; --- Trampoline location in RAM ---
EXT_TRAMPOLINE  = $0270         ; RAM address where trampoline code is copied
EXT_TRAMP_SIZE  = 14            ; size of trampoline code in bytes

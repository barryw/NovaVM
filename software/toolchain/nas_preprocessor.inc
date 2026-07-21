; Stable mailbox ABI shared by NAS and its disk-loaded preprocessing overlay.

.ifndef NAS_PREPROCESSOR_INC
NAS_PREPROCESSOR_INC = 1

.include "libabi.inc"

NASPP_LOAD     = $7000
NASPP_MAX      = $3000
NASPP_NAME_LEN = 10

; SYS_OVL_MAIN leaves LIB_ARG0..3 intact while calling the overlay.
NASPP_INPUT_XADDR  = LIB_ARG0     ; u24
NASPP_INPUT_LEN    = LIB_ARG1     ; u16; overlay replaces with output length
NASPP_OUTPUT_XADDR = LIB_ARG2     ; u24
NASPP_OUTPUT_CAP   = LIB_ARG3     ; u16
NASPP_ERROR        = LIB_ARG0+3   ; overlay parser error, zero on success
NASPP_ERROR_LINE   = LIB_ARG1+2   ; u16 original-source line
NASPP_ERROR_COL    = LIB_ARG3+2   ; u16 (currently directive start)
NASPP_DEFINE_LEN   = $02F0        ; optional NAME=VALUE definition
NASPP_DEFINE       = $0970

NASPP_ERR_SYNTAX   = 1
NASPP_ERR_CAPACITY = 2
NASPP_ERR_NESTING  = 1
NASPP_ERR_LINE     = 4
NASPP_ERR_OUTPUT   = 2

.endif

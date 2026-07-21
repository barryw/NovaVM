; Stable mailbox ABI for NAS's disk-loaded opcode-table backend.

.ifndef NAS_BACKEND_INC
NAS_BACKEND_INC = 1

.include "libabi.inc"

NASBE_LOAD       = $7000
NASBE_ENTRY      = NASBE_LOAD
NASBE_MAX        = $3000
NASBE_NAME_LEN   = 10

NASBE_MODE       = LIB_ARG0
NASBE_MNEMONIC   = LIB_ARG1

NASBE_IMPLIED    = 0
NASBE_IMMEDIATE  = 1
NASBE_ACCUMULATOR= 2
NASBE_ZP         = 3
NASBE_ZPX        = 4
NASBE_ZPY        = 5
NASBE_ABS        = 6
NASBE_ABSX       = 7
NASBE_ABSY       = 8
NASBE_INDX       = 9
NASBE_INDY       = 10
NASBE_ZPIND      = 11
NASBE_ABSIND     = 12
NASBE_ABSINDX    = 13
NASBE_MODE_COUNT = 14

.endif

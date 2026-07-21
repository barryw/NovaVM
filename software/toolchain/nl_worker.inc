; Fixed mailbox shared by resident NL and its disk-loaded worker overlay.

.ifndef NL_WORKER_INC
NL_WORKER_INC = 1

NLW_COMMAND       = $0900
NLW_OBJECT_COUNT  = $0901
NLW_ROOT_COUNT    = $0902
NLW_ERROR         = $0903
NLW_OBJECT_PTR_L  = $0904              ; 8 bytes
NLW_OBJECT_PTR_H  = $090C              ; 8 bytes
NLW_LOAD_BASE     = $0914              ; u16
NLW_OUTPUT_LEN    = $0916              ; u16
NLW_SEC_OFFSET_L  = $0918              ; 64 bytes
NLW_SEC_OFFSET_H  = $0958              ; 64 bytes
NLW_SEC_SIZE_L    = $0998              ; 64 bytes
NLW_SEC_SIZE_H    = $09D8              ; 64 bytes
NLW_MAP_PTR       = $0A18              ; u16
NLW_MAP_CAP       = $0A1A              ; u16
NLW_MAP_LEN       = $0A1C              ; u16
NLW_LIVE_MASK     = $0A1E              ; one 8-section bitmap per object
NLW_GC_READY      = $0A26              ; nonzero when LIVE_MASK is valid
NLW_OBJECT_LEN_L  = $0A27              ; 8 bytes
NLW_OBJECT_LEN_H  = $0A2F              ; 8 bytes
NLW_LIBRARY_PTR   = $0A37              ; u16
NLW_LIBRARY_LEN   = $0A39              ; u16
NLW_CONFIG_SYMBOL_COUNT = $0A3B
NLW_CONFIG_SYMBOL_NAME_LEN = $0A3C     ; 4 bytes
NLW_CONFIG_SYMBOL_NAMES = $0A40        ; 4 * 16 bytes
NLW_CONFIG_SYMBOL_VALUE_L = $0A80      ; 4 bytes
NLW_CONFIG_SYMBOL_VALUE_H = $0A84      ; 4 bytes

NLW_COMMAND_MAP   = 1
NLW_COMMAND_GC    = 2
NLW_COMMAND_LABEL = 3
NLW_ERROR_COMMAND = 1
NLW_ERROR_OUTPUT  = 2
NLW_ERROR_OBJECT  = 3
NLW_LOAD          = $7000
; NLWORK's packed payload and BSS occupy exactly $7000-$78BF. The remainder
; of its fixed band stages two language-neutral NOBJ inputs while it runs.
NLW_MAX           = $08C0
NLW_OBJECT0       = NLW_LOAD + NLW_MAX
NLW_OBJECT0_CAP   = $1A00
NLW_OBJECT1       = NLW_OBJECT0 + NLW_OBJECT0_CAP
NLW_OBJECT1_CAP   = $0D40
NLW_NAME_LEN      = 10

.endif

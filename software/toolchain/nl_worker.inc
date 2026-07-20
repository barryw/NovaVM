; Fixed mailbox shared by resident NL and its disk-loaded worker overlay.

.ifndef NL_WORKER_INC
NL_WORKER_INC = 1

NLW_COMMAND       = $0900
NLW_OBJECT_COUNT  = $0901
NLW_ROOT_COUNT    = $0902
NLW_ERROR         = $0903
NLW_OBJECT_PTR_L  = $0904              ; 4 bytes
NLW_OBJECT_PTR_H  = $0908              ; 4 bytes
NLW_LOAD_BASE     = $090C              ; u16
NLW_OUTPUT_LEN    = $090E              ; u16
NLW_SEC_OFFSET_L  = $0910              ; 32 bytes
NLW_SEC_OFFSET_H  = $0930              ; 32 bytes
NLW_SEC_SIZE_L    = $0950              ; 32 bytes
NLW_SEC_SIZE_H    = $0970              ; 32 bytes
NLW_MAP_PTR       = $0990              ; u16
NLW_MAP_CAP       = $0992              ; u16
NLW_MAP_LEN       = $0994              ; u16
NLW_LIVE_MASK     = $0996              ; one 8-section bitmap per object
NLW_GC_READY      = $099A              ; nonzero when LIVE_MASK is valid
NLW_OBJECT_LEN_L  = $099B              ; 4 bytes
NLW_OBJECT_LEN_H  = $099F              ; 4 bytes
NLW_LIBRARY_PTR   = $09A3              ; u16
NLW_LIBRARY_LEN   = $09A5              ; u16
NLW_CONFIG_SYMBOL_COUNT = $09A7
NLW_CONFIG_SYMBOL_NAME_LEN = $09A8     ; 4 bytes
NLW_CONFIG_SYMBOL_NAMES = $09AC        ; 4 * 16 bytes
NLW_CONFIG_SYMBOL_VALUE_L = $09EC      ; 4 bytes
NLW_CONFIG_SYMBOL_VALUE_H = $09F0      ; 4 bytes

NLW_COMMAND_MAP   = 1
NLW_COMMAND_GC    = 2
NLW_COMMAND_LABEL = 3
NLW_ERROR_COMMAND = 1
NLW_ERROR_OUTPUT  = 2
NLW_ERROR_OBJECT  = 3
NLW_LOAD          = $7000
NLW_MAX           = $3000
NLW_NAME_LEN      = 10

.endif

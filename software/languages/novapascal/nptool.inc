; NovaPascal shell/tool executable ABI.

.ifndef NPTOOL_INC
NPTOOL_INC = 1

NPTOOL_LOAD = $4000
NPTOOL_MAX  = $3000

; Fixed low-RAM mailbox. Argument buffers include a trailing NUL.
NPTOOL_STATUS   = $0275
NPTOOL_DETAIL   = $0276
NPTOOL_ARG0_LEN = $0277
NPTOOL_ARG1_LEN = $0278
NPTOOL_ARG0     = $0279
NPTOOL_ARG1     = $02B9
NPTOOL_ARG_CAP  = 64

; A third filename lives outside the crowded page-two mailbox. Linkers use it
; for an ordinary NLIB input; the assembler and editor simply ignore it.
NPTOOL_ARG2_LEN = $02FF
NPTOOL_ARG2     = $0800

; Shared I/O transfer cells used by tool entry points.
NPTOOL_IO_ADDR  = $02F9
NPTOOL_IO_CAP   = $02FB
NPTOOL_IO_LEN   = $02FD

NPTOOL_OK           = 0
NPTOOL_ERR_ARGS     = 1
NPTOOL_ERR_IO       = 2
NPTOOL_ERR_COMPILE  = 3
NPTOOL_ERR_ASSEMBLE = 4
NPTOOL_ERR_LINK     = 5
NPTOOL_ERR_EDITOR   = 6
NPTOOL_ERR_NOT_TEXT = 7

.global nptool_load_arg0
.global nptool_load_arg2
.global nptool_save_arg0
.global nptool_save_arg1
.global nptool_clear_bss
.global nptool_validate_text
.global nptool_print_z
.global nptool_newline

.endif

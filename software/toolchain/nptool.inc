; Nova native disk-tool executable ABI.

.ifndef NPTOOL_INC
NPTOOL_INC = 1

NPTOOL_LOAD = $2000
NPTOOL_MAX  = $5000

; Linker placement option. The shell/project layer owns the default; NL only
; consumes this ABI cell and never hardcodes a runtime address.
NPTOOL_LINK_BASE = $0273

; Fixed low-RAM mailbox. Argument buffers include a trailing NUL.
NPTOOL_STATUS   = $0275
NPTOOL_DETAIL   = $0276
NPTOOL_ARG0_LEN = $0277
NPTOOL_ARG1_LEN = $0278
NPTOOL_ARG0     = $0279
NPTOOL_ARG1     = $02B9
NPTOOL_ARG_CAP  = 64

; Additional linker filenames live outside the crowded page-two mailbox.
; ARG2 is an ordinary NLIB input; ARG3 is an optional second NOBJ input;
; ARG4 is an optional ld65-style linker configuration; ARG5 is map output;
; ARG6 is one optional NAS definition (`NAME=VALUE`) or NL label output.
NPTOOL_ARG2_LEN = $02FF
NPTOOL_ARG2     = $0800
NPTOOL_ARG3_LEN = $02F7
NPTOOL_ARG3     = $0840
NPTOOL_ARG4_LEN = $02F2
NPTOOL_ARG4     = $0880
NPTOOL_ARG5_LEN = $02F1
NPTOOL_ARG5     = $08C0
NPTOOL_ARG6_LEN = $02F0
NPTOOL_ARG6     = $0970
NPTOOL_FLAGS    = $02EF
NPTOOL_FLAG_VALIDATE = $01

; Shared I/O transfer cells used by tool entry points.
NPTOOL_DIAG_LINE = $02F3
NPTOOL_DIAG_COL  = $02F5
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
NPTOOL_ERR_MEMORY   = 8
NPTOOL_IO_TOO_LARGE = $FE

.global nptool_load_arg0
.global nptool_load_arg2
.global nptool_load_arg3
.global nptool_load_arg4
.global nptool_save_arg0
.global nptool_save_arg1
.global nptool_save_named
.global nptool_clear_bss
.global nptool_validate_text
.global nptool_print_z
.global nptool_newline
.globalzp nptool_io_ptr
.globalzp nptool_io_aux
.globalzp nptool_io_left

.endif

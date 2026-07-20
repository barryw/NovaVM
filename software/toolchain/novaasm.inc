; Nova-native assembler API.

.ifndef NOVAASM_INC
NOVAASM_INC = 1

.include "nobj.inc"

NASM_OK            = 0
NASM_ERR_SYNTAX    = 1
NASM_ERR_OUTPUT    = 2
NASM_ERR_SYMBOL    = 3
NASM_ERR_RANGE     = 4
NASM_ERR_INCLUDE   = 5
NASM_ERR_ASSERT    = 6
NASM_INCLUDE_DEPTH = 8
NASM_FILENAME_CAP  = 64

.global nasm_assemble
.global nasm_source_ptr       ; 24-bit XRAM address
.global nasm_source_len
.global nasm_object_ptr
.global nasm_object_cap
.global nasm_object_len
.global nasm_error
.global nasm_error_line
.global nasm_error_column
.global nasm_source_name_ptr
.global nasm_source_name_len
.global nasm_include_name
.global nasm_include_name_len
.global nasm_include_ptr
.global nasm_include_len
.global nasm_include_display_ptr
.global nasm_error_name_ptr
.global nasm_error_name_len
.global nasm_include_open
.global nasm_include_close

.endif
